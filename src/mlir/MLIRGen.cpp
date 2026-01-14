#include "dlc/MLIRGen.h"
#include "dlc/ModelInfo.h"
#include "dlc/Dialect.h"

#include "mlir/IR/Builders.h"
#include "mlir/IR/BuiltinOps.h"
#include "mlir/IR/BuiltinTypes.h"
#include "mlir/IR/MLIRContext.h"
#include "mlir/IR/Verifier.h"
#include "mlir/Dialect/Func/IR/FuncOps.h"
#include "mlir/Dialect/LLVMIR/LLVMDialect.h"
#include "dlc/Parser.h"

#include "llvm/ADT/DenseMap.h"

using namespace mlir;
using namespace mlir::dlc;
using namespace ::dlc;

namespace dlc {

// Helper to convert internal Enum to MLIR Type
static mlir::Type getMlirType(mlir::OpBuilder &builder, TensorInfo::DataType type) {
    switch (type) {
        case TensorInfo::DataType::FLOAT: return builder.getF32Type();
        case TensorInfo::DataType::INT32: return builder.getI32Type();
        case TensorInfo::DataType::INT64: return builder.getI64Type();
        default: return builder.getNoneType();
    }
}

// Helper to get location (node's name)
mlir::Location getLoc(const NodeInfo &node, mlir::OpBuilder &builder) {
        // If the node has a name, use it. Otherwise, use the op_type
        std::string identifier = node.outputs.empty() ? node.op_type : node.outputs[0];
        return mlir::NameLoc::get(builder.getStringAttr(identifier));
}

mlir::OwningOpRef<mlir::ModuleOp>
mlirGen(mlir::MLIRContext &context, ::dlc::ModelInfo &model) {
    // Register Dialect
    context.getOrLoadDialect<DlcDialect>();
    context.getOrLoadDialect<mlir::func::FuncDialect>();

    // Create module
    auto module = mlir::ModuleOp::create(mlir::UnknownLoc::get(&context));

    // Builder
    mlir::OpBuilder builder(&context);

    // Create main function
    auto func = func::FuncOp::create(
        builder.getUnknownLoc(), "main", builder.getFunctionType({}, {})
    );

    // Tell the compiler to generate the _mlir_ciface_main wrapper
    func->setAttr(mlir::LLVM::LLVMDialect::getEmitCWrapperAttrName(), builder.getUnitAttr());

    Block *entry = func.addEntryBlock();
    builder.setInsertionPointToStart(entry);
    module.push_back(func);

    mlir::ImplicitLocOpBuilder b(builder.getUnknownLoc(), builder);

    // Initialize Value table: ONNX name -> MLIR value
    llvm::StringMap<mlir::Value> valueMap;

    // Walk nodes
    for (const NodeInfo &node : model.graph.nodes) {
        mlir::Location loc = getLoc(node, builder);
        b.setLoc(loc);
        if (node.op_type == "Constant") {
            // Expect one attribute: value
            const AttributeInfo &attr = node.attributes[0];
            const TensorInfo &tensor = attr.tensor;

            // Setup Type using clean shape vector and enum
            auto elementType = getMlirType(builder, tensor.elementType);
            auto type = RankedTensorType::get(tensor.shape, elementType);

            // Use the raw buffer
            auto denseAttr = DenseElementsAttr::getFromRawBuffer(
                type,
                llvm::ArrayRef<char>(tensor.rawData.data(), tensor.rawData.size())
            );
            auto constOp = b.create<ConstantOp>(type, denseAttr);
            valueMap[node.outputs[0]] = constOp.getResult();
        }

        else if (node.op_type == "Add") {
            auto lhs = valueMap.lookup(node.inputs[0]);
            auto rhs = valueMap.lookup(node.inputs[1]);

            if (!lhs || !rhs) {
                llvm::errs() << "Error: Add inputs not found for " << node.outputs[0] << "\n";
                return nullptr;
            }
            auto addOp = b.create<AddOp>(lhs, rhs);
            valueMap[node.outputs[0]] = addOp.getResult();
        }

        else {
            llvm::errs() << "Unsupported op: " << node.op_type << "\n";
            return nullptr;
        }
    }

    // Handle return values after the loop is done
    SmallVector<mlir::Value, 4> returnValues;
    SmallVector<mlir::Type, 4> returnTypes;

    for (const auto &outputInfo : model.graph.outputs) {
        if (valueMap.count(outputInfo.name)) {
            mlir::Value val = valueMap[outputInfo.name];
            returnValues.push_back(val);
            returnTypes.push_back(val.getType());
        } else {
            llvm::errs() << "Error: Graph output '" << outputInfo.name << "' not found!\n";
            return nullptr;
        }
    }
    b.create<func::ReturnOp>(returnValues);
    func.setType(builder.getFunctionType({}, returnTypes));

    // Verify module
    if (failed(mlir::verify(module))) {
        llvm::errs() << "MLIR verification failed\n";
        return nullptr;
    }
    return module;
}
}   // namespace dlc