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
        rewriter.replaceOpWithNewOp<arith::ConstantOp>(op, op.getValue());
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

        // 1. Create the destination tensor (required by Linalg on Tensors)
        // For phase 0, Create an empty tensor to hold the result.
        auto initTensor = tensor::EmptyOp::create(
            rewriter,
            loc,
            resultType,
            ValueRange{}
        );

        // 2. Map dlc.add to linalg.add
        // This handles both Rank-0 (scalar) and Rank-1 (vector) protos.
        rewriter.replaceOp(op, linalg::AddOp::create(
            rewriter,
            loc,
            resultType,
            ValueRange{adaptor.getLhs(), adaptor.getRhs()},
            ValueRange{initTensor.getResult()} 
        )
        );

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
            patterns.add<ConstantOpLowering, AddOpLowering>(&getContext());

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