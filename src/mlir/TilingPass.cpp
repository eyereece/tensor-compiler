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
        
        SmallVector<TilingInterface> targets;
        module.walk([&](TilingInterface op) {
            if (auto genericOp = dyn_cast<linalg::GenericOp>(op.getOperation())) {
                if (genericOp.getNumLoops() == 2) targets.push_back(op);
            }
        });
        
        for (TilingInterface consumer : targets) {
            scf::SCFTileAndFuseOptions fuseOptions;
            scf::SCFTilingOptions tilingOptions;
            
            tilingOptions.setTileSizes({rewriter.getIndexAttr(64), 
                                        rewriter.getIndexAttr(64)});
            
            fuseOptions.setFusionControlFn([](tensor::ExtractSliceOp candidateSliceOp, 
                                              OpResult originalProducer, 
                                              bool isDestination) 
                                              -> std::optional<scf::SCFTileAndFuseOptions::ControlFnResult> {
                if (isa<linalg::TransposeOp>(originalProducer.getOwner()))
                    return std::nullopt;
                    
                return scf::SCFTileAndFuseOptions::ControlFnResult{};
            });

            fuseOptions.setTilingOptions(tilingOptions);
            rewriter.setInsertionPoint(consumer);
            
            auto result = scf::tileConsumerAndFuseProducersUsingSCF(rewriter, consumer, fuseOptions);

            if (succeeded(result)) {
                for (Operation *tiledOp : result->tiledAndFusedOps) {
                    auto tilingInterfaceOp = dyn_cast<TilingInterface>(tiledOp);
                    if (tilingInterfaceOp && tiledOp->getNumOperands() > 1) { 
                        if (auto gen = dyn_cast<linalg::GenericOp>(tiledOp)) {
                            if (gen.getNumLoops() == 3) {
                                scf::SCFTilingOptions kOptions;
                                kOptions.setTileSizes({rewriter.getIndexAttr(0), 
                                                       rewriter.getIndexAttr(0), 
                                                       rewriter.getIndexAttr(64)});
                                
                                rewriter.setInsertionPoint(tiledOp);
                                
                                // FIX: Wrap in (void) or check the result to silence [[nodiscard]] warning
                                (void)scf::tileUsingSCF(rewriter, tilingInterfaceOp, kOptions);
                            }
                        }
                    }
                }

                for (const auto &it : result->replacements) {
                    rewriter.replaceAllUsesWith(it.first, it.second);
                }
                rewriter.eraseOp(consumer);
            }
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