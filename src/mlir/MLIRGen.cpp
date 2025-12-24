#include "dlc/MLIRGen.h"
#include "dlc/ModelInfo.h"
#include "dlc/Dialect.h"

#include "mlir/IR/Builders.h"
#include "mlir/IR/BuiltinOps.h"
#include "mlir/IR/BuiltinTypes.h"
#include "mlir/IR/MLIRContext.h"
#include "mlir/IR/Verifier.h"
#include "mlir/Dialect/Func/IR/FuncOps.h"
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
    context.getOrLoadDialect<mlir::func::FuncDialect>();

    // Create module
    auto module = mlir::ModuleOp::create(mlir::UnknownLoc::get(&context));

    // Builder
    mlir::OpBuilder builder(&context);

    // TEST
    auto funcType = builder.getFunctionType({}, {});
    auto func = builder.create<func::FuncOp>(
        builder.getUnknownLoc(), "main", funcType
    );

    Block *entry = func.addEntryBlock();
    builder.setInsertionPointToStart(entry);

    module.push_back(func);
    // END TEST

    // Initialize Value table: ONNX name -> MLIR value
    llvm::StringMap<mlir::Value> valueMap;

    // Walk nodes
    for (const NodeInfo &node : model.graph.nodes) {
        // TEST
        llvm::errs() << "Node op_type: " << node.op_type << "\n";
        llvm::errs() << "Inputs: ";
        for (auto &in : node.inputs)
            llvm::errs() << in << " ";
        llvm::errs() << "\nOutputs: ";
        for (auto &out : node.outputs)
            llvm::errs() << out << " ";
        llvm::errs() << "\n";
        assert(!node.outputs.empty() && "Node outputs cannot be empty");

        // END TEST
        if (node.op_type == "Constant") {
            // Expect one attribute: value
            const AttributeInfo &attr = node.attributes[0];
            assert(attr.type == AttributeInfo::TENSOR);

            // TEST
            // Print shape and data size
            llvm::errs() << "Constant tensor shape: ";
            for (auto d : attr.tensor.dims())
                llvm::errs() << d << " ";
            llvm::errs() << "\nFloat data size: " << attr.tensor.float_data().size() << "\n";

            // END TEST
            
            // Build tensor type
            SmallVector<int64_t> shape;
            for (auto d : attr.tensor.dims())
                shape.push_back(d);

            // TEST
            // Check shape matches data size
            int64_t expected_size = 1;
            for (auto d : shape)
                expected_size *= d;
            if (expected_size != (int64_t)attr.tensor.float_data().size()) {
                llvm::errs() << "Error: Tensor shape does not match data size\n";
                return nullptr;
            }

            // END TEST

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

            // TEST
            llvm::errs() << "Inserted value for output: " << node.outputs[0] << " with type: " << valueMap[node.outputs[0]].getType() << "\n";
            // END TEST
        }

        else if (node.op_type == "Add") {
            auto lhs = valueMap.lookup(node.inputs[0]);
            auto rhs = valueMap.lookup(node.inputs[1]);

            if (!lhs || !rhs) {
                llvm::errs() << "Error: Add inputs not found in valueMap for " << node.outputs[0] << "\n";
                return nullptr;
            }

            // TEST
            llvm::errs() << "AddOp lhs type: " << lhs.getType() << "\n";
            llvm::errs() << "AddOp rhs type: " << rhs.getType() << "\n";

            // END TEST

            auto addOp =
                builder.create<AddOp>(builder.getUnknownLoc(), lhs, rhs);

            valueMap[node.outputs[0]] = addOp.getResult();

            // TEST
            llvm::errs() << "Inserted value for output: " << node.outputs[0] << " with type: " << valueMap[node.outputs[0]].getType() << "\n";
            // END TEST
        }

        else {
            llvm::errs() << "Unsupported op: " << node.op_type << "\n";
            return nullptr;
        }
    }

    // Handle return values after the loop is done
    SmallVector<mlir::Value, 4> returnValues;
    SmallVector<mlir::Type, 4> returnTypes;

    for (const auto &outputProto : model.graph.outputs) {
        std::string outputName = outputProto.name();
        if (valueMap.count(outputName)) {
            mlir::Value val = valueMap[outputName];
            returnValues.push_back(val);
            returnTypes.push_back(val.getType());
        } else {
            llvm::errs() << "Error: Graph output '" << outputName << "' not found!\n";
            return nullptr;
        }
    }
    builder.create<mlir::func::ReturnOp>(builder.getUnknownLoc(), returnValues);

    func.setType(builder.getFunctionType({}, returnTypes));

    // Verify module

    // TEST
    llvm::errs() << "Before verification\n";
    // END TEST

    if (failed(mlir::verify(module))) {
        llvm::errs() << "MLIR verification failed\n";
        return nullptr;
    }

    // TEST
    llvm::errs() << "After verification\n";
    // END TEST

    return module;
}
}   // namespace dlc