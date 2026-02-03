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
        auto *context = &getContext();
        IRRewriter rewriter(context);

        module.walk([&](linalg::GenericOp op) {
            // only target matmul which has 3 loops
            if (op.getNumLoops() != 3)
                return WalkResult::advance();

            // Make sure it's a TilingInterface
            auto tilingOp = dyn_cast<TilingInterface>(op.getOperation());
            if (!tilingOp)
                return WalkResult::advance();;

            // Get the loop ranges
            SmallVector<Range> iterationDomain = tilingOp.getIterationDomain(rewriter);

            // Threshold check
            int64_t minTileSize = 64;
            bool worthTiling = false;

            for (int i = 0; i < (int)iterationDomain.size(); ++i) {
                auto constantBound = getConstantIntValue(iterationDomain[i].size);

                // if it's dynamic or large (>64), it's worth tiling
                if (!constantBound || *constantBound > minTileSize) {
                    worthTiling = true;
                    break;
                }
            }

            // IF the matrix is tiny, just keep walking
            if (!worthTiling)
                return WalkResult::advance();
            
            scf::SCFTilingOptions options;

            SmallVector<OpFoldResult> tileSizes;
            tileSizes.push_back(rewriter.getIndexAttr(64)); // M
            tileSizes.push_back(rewriter.getIndexAttr(64)); // N
            tileSizes.push_back(rewriter.getIndexAttr(64)); // K

            options.setTileSizes(tileSizes);

            rewriter.setInsertionPointAfter(op);
            FailureOr<scf::SCFTilingResult> TilingResult =
                scf::tileUsingSCF(rewriter, tilingOp, options);

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