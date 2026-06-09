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
        // Register dialects so the pass knows how to create loops/slices
        registry.insert<linalg::LinalgDialect, scf::SCFDialect,
                        tensor::TensorDialect, arith::ArithDialect>();

        linalg::registerTilingInterfaceExternalModels(registry);
    }

    void runOnOperation() override {
        auto module = getOperation();
        IRRewriter rewriter(&getContext());
        
        // Target matmul
        SmallVector<TilingInterface> targets;
        module.walk([&](TilingInterface op) {
            // Find the ReLU/Add (the 2D consumer)
            if (auto genericOp = dyn_cast<linalg::GenericOp>(op.getOperation())) {
                if (!genericOp) return;
                if (genericOp.getNumLoops() == 3 && genericOp.getNumReductionLoops() == 1)
                    targets.push_back(op);
            }

        });

        // Tile matmul
        for (auto target : targets) {
            scf::SCFTilingOptions options;

            options.setTileSizes({
                rewriter.getIndexAttr(64),
                rewriter.getIndexAttr(64),
                rewriter.getIndexAttr(64)
            });

            rewriter.setInsertionPoint(target);

            auto result = 
                scf::tileUsingSCF(
                    rewriter,
                    target,
                    options
                );

            if (failed(result)) continue;

            rewriter.replaceOp(
                target.getOperation(),
                result->replacements
            );
        }
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