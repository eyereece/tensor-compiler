// Passes.h (Dlc Passes Definition)

#ifndef DLC_PASSES_H
#define DLC_PASSES_H

#include <memory>

namespace mlir {
class Pass;

namespace dlc {
std::unique_ptr<mlir::Pass> createLowerToTensorPass();
std::unique_ptr<mlir::Pass> createLowerToLLVMPass();
std::unique_ptr<mlir::Pass> createLinalgTilingPass();
}   // namespace dlc
}   // namespace mlir
#endif // DLC_PASSES_H