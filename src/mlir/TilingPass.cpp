#include "mlir/Dialect/Linalg/Transforms/Transforms.h"
#include "mlir/Interfaces/TilingInterface.h"
#include "mlir/Dialect/SCF/Transforms/TileUsingInterface.h"
#include "mlir/Dialect/Linalg/IR/Linalg.h"
#include "mlir/Dialect/SCF/IR/SCF.h"
#include "mlir/Dialect/Arith/IR/Arith.h"
#include "mlir/Dialect/Tensor/IR/Tensor.h"
#include "mlir/IR/PatternMatch.h"
#include "mlir/Pass/Pass.h"
#include "mlir/Dialect/Linalg/TransformOps/LinalgTransformOps.h"
#include "mlir/Dialect/Linalg/IR/LinalgInterfaces.h"
#include "mlir/Dialect/Linalg/Transforms/TilingInterfaceImpl.h"

using namespace mlir;

namespace {
struct LinalgTilingPass : public PassWrapper<LinalgTilingPass, OperationPass<ModuleOp>> {
    MLIR_DEFINE_EXPLICIT_INTERNAL_INLINE_TYPE_ID(LinalgTilingPass)

    void getDependentDialects(DialectRegistry &registry) const override {
        // You must register these dialects so the pass knows how to create loops/slices
        registry.insert<linalg::LinalgDialect, scf::SCFDialect,
                        tensor::TensorDialect, arith::ArithDialect>();

        linalg::registerTilingInterfaceExternalModels(registry);
    }

    void runOnOperation() override {
        auto module = getOperation();
        auto *context = &getContext();
        IRRewriter rewriter(context);

        module.walk([&](TilingInterface op) {
            // Target only our matmul
            if (!isa<linalg::MatmulOp>(op.getOperation()))
                return WalkResult::advance();

            
            scf::SCFTilingOptions options;

            // For 512x512 model, use 32x32 tiles
            SmallVector<OpFoldResult> tileSizes;
            tileSizes.push_back(rewriter.getIndexAttr(32)); // M
            tileSizes.push_back(rewriter.getIndexAttr(32)); // N
            // tileSizes.push_back(rewriter.getIndexAttr(32)); // K

            options.setTileSizes(tileSizes);

            rewriter.setInsertionPointAfter(op);
            FailureOr<scf::SCFTilingResult> TilingResult =
                scf::tileUsingSCF(rewriter, op, options);

            if (succeeded(TilingResult)) {
                rewriter.replaceOp(op, TilingResult->replacements);
                return WalkResult::advance();
            }

            return WalkResult::interrupt();
        });
    }
};
}   // namespace

namespace mlir {
namespace dlc {
    std::unique_ptr<Pass> createLinalgTilingPass() {
        return std::make_unique<LinalgTilingPass>();
    }
}   // namespace dlc
}   // namespace mlir