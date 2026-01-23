#include "dlc/Dialect.h"

#include "mlir/Dialect/Arith/IR/Arith.h"
#include "mlir/Dialect/Func/IR/FuncOps.h"
#include "mlir/Dialect/Linalg/IR/Linalg.h"
#include "mlir/Dialect/Tensor/IR/Tensor.h"
#include "mlir/Pass/Pass.h"
#include "mlir/Transforms/DialectConversion.h"

using namespace mlir;
using namespace mlir::dlc;

namespace {

//===----------------------------------------------------------------------===//
// Pattern: dlc.constant -> arith.constant
//===----------------------------------------------------------------------===//
struct ConstantOpLowering : public OpConversionPattern<dlc::ConstantOp> {
    using OpConversionPattern<dlc::ConstantOp>::OpConversionPattern;

    LogicalResult matchAndRewrite(dlc::ConstantOp op, OpAdaptor adaptor,
                                ConversionPatternRewriter &rewriter) const final {
        // Direcly replace dlc.constant with arith.constant
        // Both use DenseElementsAttr, so this is a 1-to-1 mapping
        auto newOp = arith::ConstantOp::create(rewriter, op.getLoc(), op.getValue());
        rewriter.replaceOp(op, newOp);
        return success();
    }
};

//===----------------------------------------------------------------------===//
// Pattern: dlc.add -> linalg.add
//===----------------------------------------------------------------------===//
struct AddOpLowering : public OpConversionPattern<dlc::AddOp> {
    using OpConversionPattern<dlc::AddOp>::OpConversionPattern;

    LogicalResult matchAndRewrite(dlc::AddOp op, OpAdaptor adaptor,
                                ConversionPatternRewriter &rewriter) const final {
        auto loc = op.getLoc();
        auto resultType = cast<RankedTensorType>(op.getType());

        // Create the destination tensor (required by Linalg on Tensors)
        auto emptyTensor = tensor::EmptyOp::create(
            rewriter,
            loc,
            resultType.getShape(),
            resultType.getElementType()
        );

        // Create zero constant for the element type
        auto zeroAttr = rewriter.getFloatAttr(resultType.getElementType(), 0.0);
        auto zeroConstant = arith::ConstantOp::create(rewriter, loc, zeroAttr);

        auto initTensor = linalg::FillOp::create(
            rewriter,
            loc,
            ValueRange{zeroConstant},
            ValueRange{emptyTensor.getResult()}
        );


        // Map dlc.add to linalg.add
        // This handles both Rank-0 (scalar) and Rank-1 (vector) protos.
        auto addOp = linalg::AddOp::create(
            rewriter,
            loc,
            TypeRange{resultType},                          // Result Types
            ValueRange{adaptor.getLhs(), adaptor.getRhs()}, // ins
            ValueRange{initTensor.getResult(0)}                 // outs
        );
        rewriter.replaceOp(op, addOp->getResults());
        return success();
    }
};

//===----------------------------------------------------------------------===//
// Pattern: dlc.relu -> linalg.generic
//===----------------------------------------------------------------------===//
struct ReluOpLowering : public OpConversionPattern<dlc::ReluOp> {
    using OpConversionPattern<dlc::ReluOp>::OpConversionPattern;

    LogicalResult matchAndRewrite(dlc::ReluOp op, OpAdaptor adaptor,
                                ConversionPatternRewriter &rewriter) const final {
        auto loc = op.getLoc();
        auto type = cast<RankedTensorType>(op.getType());

        // Create destination tensor
        auto initTensor = tensor::EmptyOp::create(
            rewriter, loc, type.getShape(), type.getElementType()
        );

        // Define the indexing maps (identity for element-wise)
        auto indexingMap = rewriter.getMultiDimIdentityMap(type.getRank());
        SmallVector<AffineMap> maps(2, indexingMap);    // one for input, one for output

        // Define iterator types (all parallel for element-wise)
        SmallVector<utils::IteratorType> iterators(type.getRank(), utils::IteratorType::parallel);

        // Create linalg.generic op
        auto reluGeneric = linalg::GenericOp::create(
            rewriter,
            loc,
            type,
            /*inputs=*/adaptor.getInput(),
            /*outputs=*/initTensor.getResult(),
            maps,
            iterators,
            [&](OpBuilder &nestedBuilder, Location nestedLoc, ValueRange args) {
                // max(0, input)
                Value input = args[0];
                Value zero = arith::ConstantOp::create(nestedBuilder, nestedLoc, nestedBuilder.getFloatAttr(type.getElementType(), 0.0));
                Value max = arith::MaxNumFOp::create(nestedBuilder, nestedLoc, input, zero);
                linalg::YieldOp::create(nestedBuilder, nestedLoc, max);
            }
        );
    
    // Use attributes to tag it as relu
    // reluGeneric->setAttr("relu", rewriter.getUnitAttr());
    rewriter.replaceOp(op, reluGeneric->getResults());
    return success();
    }
};

//===----------------------------------------------------------------------===//
// Pattern: dlc.matmul -> linalg.matmul
//===----------------------------------------------------------------------===//
struct MatMulOpLowering : public OpConversionPattern<dlc::MatMulOp> {
    using OpConversionPattern<dlc::MatMulOp>::OpConversionPattern;

    LogicalResult matchAndRewrite(dlc::MatMulOp op, OpAdaptor adaptor,
                                ConversionPatternRewriter &rewriter) const final {
        auto loc = op.getLoc();
        auto lhs = adaptor.getLhs();
        auto rhs = adaptor.getRhs();

        // Handle Rank-1 to Rank-2 promotion for LHS [K] -> [1, K]
        if (llvm::cast<RankedTensorType>(lhs.getType()).getRank() == 1) {
            auto shape = llvm::cast<RankedTensorType>(lhs.getType()).getShape();
            auto newType = RankedTensorType::get({1, shape[0]},
                                                llvm::cast<RankedTensorType>(lhs.getType()).getElementType());
            
            // Reassociation maps from [dim] -> [0, dim]
            SmallVector<ReassociationIndices> reassoc = {{0, 1}};
            lhs = tensor::ExpandShapeOp::create(rewriter, loc, newType, lhs, reassoc);
        }

        // Handle Rank-1 to Rank-2 promotion for RHS [K] -> [K, 1]
        if (llvm::cast<RankedTensorType>(rhs.getType()).getRank() == 1) {
            auto shape = llvm::cast<RankedTensorType>(rhs.getType()).getShape();
            auto newType = RankedTensorType::get({shape[0], 1},
                                                llvm::cast<RankedTensorType>(rhs.getType()).getElementType());

            SmallVector<ReassociationIndices> reassoc = {{0, 1}};
            rhs = tensor::ExpandShapeOp::create(rewriter, loc, newType, rhs, reassoc);
        }

        auto lhsType = cast<RankedTensorType>(lhs.getType());
        auto rhsType = cast<RankedTensorType>(rhs.getType());

        int64_t M = lhsType.getShape()[0];
        int64_t N = rhsType.getShape()[1];

        auto resultType = RankedTensorType::get({M, N}, lhsType.getElementType());

        // Create the uninitialized allocation
        auto emptyTensor = tensor::EmptyOp::create(
            rewriter,
            loc,
            resultType.getShape(),
            resultType.getElementType()
        );

        // Create a zero constant for the element type
        auto zeroAttr = rewriter.getFloatAttr(resultType.getElementType(), 0.0);
        auto zeroConstant = arith::ConstantOp::create(rewriter, loc, zeroAttr);

        // Create the destination tensor for Destination Passing Style (DPS)
        auto initTensor = linalg::FillOp::create(
            rewriter,
            loc,
            ValueRange{zeroConstant},
            ValueRange{emptyTensor.getResult()}
        );

        // Lower to linalg.matmul
        auto matmulOp = linalg::MatmulOp::create(
            rewriter,
            loc,
            resultType,
            ValueRange{lhs, rhs},   // ins
            ValueRange{initTensor.getResult(0)}
        );
        
        Value result = matmulOp.getResult(0);

        if (op.getType().getRank() == 1) {
            // Collapse [1, N] -> [N]
            SmallVector<ReassociationIndices> reassoc = {{0, 1}};
            result = tensor::CollapseShapeOp::create(
                rewriter, loc, op.getType(), result, reassoc
            );
        }

        rewriter.replaceOp(op, result);
        return success();
    }
};

//===----------------------------------------------------------------------===//
// Pass Definition
//===----------------------------------------------------------------------===//
struct DlcToTensorLoweringPass
    : public PassWrapper<DlcToTensorLoweringPass, OperationPass<ModuleOp>> {
        MLIR_DEFINE_EXPLICIT_INTERNAL_INLINE_TYPE_ID(DlcToTensorLoweringPass)

        void getDependentDialects(DialectRegistry &registry) const override {
            registry.insert<arith::ArithDialect, linalg::LinalgDialect,
                            tensor::TensorDialect, func::FuncDialect>();
        }

        void runOnOperation() final {
            ConversionTarget target(getContext());

            // Define what is legal
            target.addLegalDialect<arith::ArithDialect, linalg::LinalgDialect,
                                    tensor::TensorDialect, func::FuncDialect>();

            // Mark dlc dialect as illegal
            target.addIllegalDialect<dlc::DlcDialect>();

            RewritePatternSet patterns(&getContext());
            patterns.add<ConstantOpLowering, AddOpLowering, ReluOpLowering, MatMulOpLowering>(&getContext());

            if (failed(applyPartialConversion(getOperation(), target, std::move(patterns))))
                signalPassFailure();
        }
    };
} // namespace

namespace mlir {
namespace dlc {
std::unique_ptr<Pass> createLowerToTensorPass() {
    return std::make_unique<DlcToTensorLoweringPass>();
}
}   // namespace dlc
}   // namespace mlir