#include "dlc/MLIRGen.h"
#include "dlc/ModelInfo.h"
#include "dlc/Dialect.h"

#include "mlir/IR/Builders.h"
#include "mlir/IR/BuiltinOps.h"
#include "mlir/IR/BuiltinTypes.h"
#include "mlir/IR/MLIRContext.h"
#include "mlir/IR/Verifier.h"
#include "dlc/Parser.h"

#include "llvm/ADT/DenseMap.h"

using namespace mlir;
using namespace mlir::dlc;
using namespace ::dlc;

namespace dlc {
mlir::OwningOpRef<mlir::ModuleOp>
mlirGen(mlir::MLIRContext &context, ::dlc::ModelInfo &model) {
    // Register Dialect
    context.getOrLoadDialect<DlcDialect>();

    // Create module
    auto module = mlir::ModuleOp::create(mlir::UnknownLoc::get(&context));

    // Builder
    mlir::OpBuilder builder(&context);

    // Value table: ONNX name -> MLIR value
    llvm::StringMap<mlir::Value> valueMap;

    // Walk nodes
    for (const NodeInfo &node : model.graph.nodes) {
        if (node.op_type == "Constant") {
            // Expect one attribute: value
            const AttributeInfo &attr = node.attributes[0];
            assert(attr.type == AttributeInfo::TENSOR);
            
            // Build tensor type
            SmallVector<int64_t> shape;
            for (auto d : attr.tensor.dims())
                shape.push_back(d);

            auto type = RankedTensorType::get(
                shape, builder.getF32Type()
            );

            // Convert Protobug RepeatedField to llvm::ArrayRef
            auto data = attr.tensor.float_data();
            auto denseAttr = DenseElementsAttr::get(type,
                                        llvm::ArrayRef<float>(data.data(), data.size())
            );

            auto constOp =
                builder.create<ConstantOp>(builder.getUnknownLoc(), denseAttr);

            valueMap[node.outputs[0]] = constOp.getResult();
        }

        else if (node.op_type == "Add") {
            auto lhs = valueMap.lookup(node.inputs[0]);
            auto rhs = valueMap.lookup(node.inputs[1]);

            if (!lhs || !rhs) {
                llvm::errs() << "Error: Add inputs not found in valueMap for " << node.outputs[0] << "\n";
                return nullptr;
            }

            auto addOp =
                builder.create<AddOp>(builder.getUnknownLoc(), lhs, rhs);

            valueMap[node.outputs[0]] = addOp.getResult();
        }

        else {
            llvm::errs() << "Unsupported op: " << node.op_type << "\n";
            return nullptr;
        }
    }

    // Verify module
    if (failed(mlir::verify(module))) {
        llvm::errs() << "MLIR verification failed\n";
        return nullptr;
    }

    return module;
}
}   // namespace dlc