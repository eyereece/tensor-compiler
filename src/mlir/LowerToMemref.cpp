
#include "mlir/IR/BuiltinOps.h"
#include "mlir/Dialect/Func/IR/FuncOps.h"
#include "mlir/IR/Builders.h"
#include "mlir/Pass/Pass.h"
#include "mlir/IR/MLIRContext.h"
#include "mlir/IR/TypeUtilities.h"
#include "mlir/IR/Types.h"
#include "mlir/IR/TypeUtilities.h"

#include "llvm/Support/Casting.h"

using namespace mlir;

namespace {
struct LowerToMemrefPass :
    public PassWrapper<LowerToMemrefPass, OperationPass<ModuleOp>> {
        void runOnOperation() override {
            ModuleOp module = getOperation();
            MLIRContext *context = &getContext();
            OpBuilder builder(context);

            // Iterate over all functions in the module
            for (auto funcOp : module.getOps<func::FuncOp>()) {
                // get old function type
                auto funcType = funcOp.getFunctionType();

                SmallVector<Type, 4> newInputs;
                SmallVector<Type, 4> newResults;

                // Convert tensor types in inputs to memref types, leave others unchanged
                for (Type t : funcType.getInputs()) {
                    if (auto tensorType = dyn_cast<RankedTensorType>(t)) {
                        // Create memref type with same shape and element type
                        auto memrefType = MemRefType::get(tensorType.getShape(), tensorType.getElementType());
                        newInputs.push_back(memrefType);
                    } else {
                        newInputs.push_back(t);
                    }
                }

                // Convert tensor types in results to memref types
                for (Type t : funcType.getResults()) {
                    if (auto tensorType = dyn_cast<RankedTensorType>(t)) {
                        auto memrefType = MemRefType::get(tensorType.getShape(), tensorType.getElementType());
                        newResults.push_back(memrefType);
                    } else {
                        newResults.push_back(t);
                    }
                }

                // Build new function type
                auto newFuncType = FunctionType::get(context, newInputs, newResults);

                // Update function signature
                funcOp.setType(newFuncType);
            }
        }
    };
}   // namespace

namespace mlir {
namespace dlc {
std::unique_ptr<Pass> createLowerToMemrefPass() {
    return std::make_unique<LowerToMemrefPass>();
}
}   // namespace dlc
}   // namespace mlir