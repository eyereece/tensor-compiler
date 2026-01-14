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

    llvm::SmallVector<mlir::Type, 4> argTypes;
    for (const auto &inputInfo : model.graph.inputs) {
        auto elementType = getMlirType(builder, inputInfo.elementType);
        argTypes.push_back(mlir::RankedTensorType::get(inputInfo.shape, elementType));
    }

    // Create main function
    auto func = func::FuncOp::create(
        builder.getUnknownLoc(), "main", builder.getFunctionType(argTypes, {})
    );

    // Tell the compiler to generate the _mlir_ciface_main wrapper
    func->setAttr(mlir::LLVM::LLVMDialect::getEmitCWrapperAttrName(), builder.getUnitAttr());

    Block *entry = func.addEntryBlock();
    builder.setInsertionPointToStart(entry);
    module.push_back(func);

    mlir::ImplicitLocOpBuilder b(builder.getUnknownLoc(), builder);

    // Initialize Value table: ONNX name -> MLIR value
    llvm::StringMap<mlir::Value> valueMap;

    for (size_t i = 0; i < model.graph.inputs.size(); ++i) {
const auto &inputInfo = model.graph.inputs[i];
        
        // --- TEMPORARY HACK TO PREVENT SEGFAULT ---
        // Instead of using the function argument, create a constant
        // This allows the JIT to run without passing data from C++
        auto elementType = getMlirType(builder, inputInfo.elementType);
        auto type = RankedTensorType::get(inputInfo.shape, elementType);
        
        // dummy data
        // For float32, [0.0, 0.0]
        std::vector<float> dummyData(2, 5.0f);
        auto denseAttr = DenseElementsAttr::get(type, llvm::ArrayRef<float>(dummyData));
        
        auto constOp = b.create<ConstantOp>(type, denseAttr);
        valueMap[inputInfo.name] = constOp.getResult();
        // ------------------------------------------
        
        // valueMap[inputInfo.name] = entry->getArgument(i);
    }

    // Walk nodes
    for (const NodeInfo &node : model.graph.nodes) {
        mlir::Location loc = getLoc(node, builder);
        b.setLoc(loc);
        // TEMP
        llvm::SmallVector<mlir::Value, 2> operands;
        for (const std::string &inName : node.inputs) {
            if (valueMap.count(inName)) {
                operands.push_back(valueMap[inName]);
            } else if (model.graph.initializers.count(inName)) {
                const auto &tensor = model.graph.initializers[inName];
                auto type = RankedTensorType::get(tensor.shape, getMlirType(builder, tensor.elementType));

                auto denseAttr = DenseElementsAttr::getFromRawBuffer(
                    type, llvm::ArrayRef<char>(tensor.rawData.data(), tensor.rawData.size())
                );

                auto constOp = b.create<ConstantOp>(type, denseAttr);
                valueMap[inName] = constOp.getResult();
                operands.push_back(constOp.getResult());
            }
        }
        if (node.op_type == "Add") {
            valueMap[node.outputs[0]] = b.create<AddOp>(operands[0], operands[1]);
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
    entry->eraseArguments(0, entry->getNumArguments());
    func.setType(builder.getFunctionType({}, returnTypes));

    // Verify module
    if (failed(mlir::verify(module))) {
        llvm::errs() << "MLIR verification failed\n";
        return nullptr;
    }
    return module;
}
}   // namespace dlc