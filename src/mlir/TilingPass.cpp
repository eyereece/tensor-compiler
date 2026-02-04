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

        module.walk([&](linalg::GenericOp op) {
            if (op.getNumLoops() != 3)
                return WalkResult::advance();

            auto tilingOp = dyn_cast<TilingInterface>(op.getOperation());
            if (!tilingOp) return WalkResult::advance();

            SmallVector<Range> iterationDomain = tilingOp.getIterationDomain(rewriter);
            int64_t minTileSize = 64;
            bool worthTiling = false;

            for (auto range : iterationDomain) {
                if (auto constantBound = getConstantIntValue(range.size)) {
                    if (*constantBound > minTileSize) {
                        worthTiling = true;
                        break;
                    }
                }
            }

            if (!worthTiling)
                return WalkResult::advance();

            // Tile + Fuse
            scf::SCFTilingOptions options;
            options.setTileSizes({rewriter.getIndexAttr(64),
                                rewriter.getIndexAttr(64),
                                rewriter.getIndexAttr(64)});

            // Use simple tileUsingSCF to get 3 loops without aggressive fusion
            auto tilingResult = scf::tileUsingSCF(rewriter, tilingOp, options);
            if (failed(tilingResult)) return WalkResult::interrupt();

            Operation* tiledMatmul = tilingResult->tiledOps[0];
            Value initOperand = tiledMatmul->getOperand(2);     // outs operand

            if (auto fillOp = initOperand.getDefiningOp<linalg::FillOp>()) {
                fillOp->moveBefore(tilingResult->loops[2]);
            }

            rewriter.replaceOp(op, tilingResult->replacements[0]);
            return WalkResult::advance();
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