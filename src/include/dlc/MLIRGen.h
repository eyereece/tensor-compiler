#ifndef DLC_MLIRGEN_H
#define DLC_MLIRGEN_H

#include <memory>

namespace mlir {
class MLIRContext;
template <typename OpTy>
class OwningOpRef;
class ModuleOp;
}   // namespace mlir

namespace dlc {
class ModelInfo;

// Emit IR for the given Dlc ModelInfo, returns a newly created MLIR module
mlir::OwningOpRef<mlir::ModuleOp> mlirGen(mlir::MLIRContext &context,
                                        ModelInfo &model);
}   // namespace dlc

#endif  // DLC_MLIRGEN_H