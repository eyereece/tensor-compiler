#include "dlc/ModelInfo.h"
#include "dlc/Parser.h"
#include "dlc/MLIRGen.h"
#include "dlc/Dialect.h"
#include "dlc/Passes.h"

#include <onnx/onnx.pb.h>

#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <google/protobuf/text_format.h>

#include "llvm/ADT/StringRef.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/MemoryBuffer.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/Format.h"

#include "mlir/Pass/PassManager.h"
#include "mlir/Transforms/Passes.h"
#include "mlir/IR/MLIRContext.h"
#include "mlir/IR/BuiltinOps.h"
#include "mlir/IR/AsmState.h"
#include "mlir/Support/LogicalResult.h"
#include "mlir/Dialect/Linalg/IR/Linalg.h"
#include "mlir/Dialect/Tensor/IR/Tensor.h"
#include "mlir/Dialect/Func/IR/FuncOps.h"
#include "mlir/Dialect/Func/Transforms/Passes.h"
#include "mlir/Dialect/Linalg/Passes.h"
#include "mlir/Dialect/SCF/Transforms/Passes.h"

#include "mlir/Dialect/Bufferization/Transforms/OneShotAnalysis.h"
#include "mlir/Dialect/Bufferization/Transforms/OneShotModuleBufferize.h"
#include "mlir/Dialect/Bufferization/Transforms/Passes.h"
#include "mlir/Dialect/MemRef/IR/MemRef.h"
#include "mlir/Dialect/Bufferization/IR/BufferizableOpInterface.h"
#include "mlir/Dialect/Arith/Transforms/BufferizableOpInterfaceImpl.h"
#include "mlir/Dialect/Tensor/Transforms/BufferizableOpInterfaceImpl.h"
#include "mlir/Dialect/Linalg/Transforms/BufferizableOpInterfaceImpl.h"
#include "mlir/Dialect/Bufferization/Transforms/FuncBufferizableOpInterfaceImpl.h"

#include "mlir/Conversion/Passes.h"
#include "mlir/Conversion/SCFToControlFlow/SCFToControlFlow.h"
#include "mlir/Conversion/ControlFlowToLLVM/ControlFlowToLLVM.h"
#include "mlir/Conversion/FuncToLLVM/ConvertFuncToLLVM.h"
#include "mlir/Conversion/FuncToLLVM/ConvertFuncToLLVMPass.h"
#include "mlir/Conversion/MemRefToLLVM/MemRefToLLVM.h"
#include "mlir/Conversion/ArithToLLVM/ArithToLLVM.h"
#include "mlir/Conversion/ReconcileUnrealizedCasts/ReconcileUnrealizedCasts.h"
#include "mlir/Dialect/LLVMIR/LLVMDialect.h"
#include "mlir/Dialect/ControlFlow/IR/ControlFlow.h"
#include "mlir/Conversion/LLVMCommon/TypeConverter.h"
#include "mlir/Conversion/LLVMCommon/ConversionTarget.h"

#include <fstream>
#include <memory>
#include <string>
#include <ostream>

namespace cl = llvm::cl;

// COMMAND-LINE OPTIONS
static cl::opt<std::string> inputFilename(
    cl::Positional,
    cl::desc("<input onnx file>"),
    cl::Required,
    cl::value_desc("filename")
);

namespace {
enum Action { None, DumpPROTO, DumpMLIR, DumpMLIRTensor, DumpMLIRMemRef, DumpMLIRLLVM };
}

static cl::opt<enum Action>
    emitAction("emit",
                cl::desc("Select the kind of output desired"),
                cl::values(
                    clEnumValN(DumpPROTO, "proto",
                                "output the ONNX protobuf graph"),
                    clEnumValN(DumpMLIR, "mlir", "output the generated MLIR module"),
                    clEnumValN(DumpMLIRTensor, "mlir-tensor",
                                "output the MLIR dump after tensor lowering"),
                    clEnumValN(DumpMLIRMemRef, "mlir-memref",
                                "output the MLIR dump after memref lowering"),
                    clEnumValN(DumpMLIRLLVM, "mlir-llvm",
                                "output the MLIR-LLVM dump after llvm lowering")
                ),
                cl::init(None));


static std::unique_ptr<onnx::ModelProto>
loadONNXModel(llvm::StringRef filename) {
    std::ifstream input(filename.str(), std::ios::binary);
    if (!input) {
        llvm::errs() << "Could not open ONNX file: " << filename << "\n";
        return nullptr;
    }

    auto model = std::make_unique<onnx::ModelProto>();
    if (!model->ParseFromIstream(&input)) {
        llvm::errs() << "Failed to parse ONNX model\n";
        return nullptr;
    }
    llvm::errs() << "Model has " << model->graph().node_size() << " nodes\n";
    return model;
}


void dumpTensor(const dlc::TensorInfo &t) {
    llvm::outs() << "        Tensor Info\n";

    // 1. Shape
    llvm::outs() << "        shape: [ ";
    if (t.shape.empty()) {
        llvm::outs() << "<scalar> ";
    } else {
        for (auto d : t.shape)
            llvm::outs() << d << " ";
    }
    llvm::outs() << "]\n";

    // 2. Data type
    llvm::outs() << "        dtype: ";
    switch (t.elementType) {
        case dlc::TensorInfo::DataType::FLOAT: llvm::outs() << "float32"; break;
        case dlc::TensorInfo::DataType::INT64: llvm::outs() << "int64"; break;
        case dlc::TensorInfo::DataType::INT32: llvm::outs() << "int32"; break;
        default: llvm::outs() << "unknown"; break;
    }
    llvm::outs() << "\n";

    // 3. Raw Data Hex Dump
    llvm::outs() << "        raw_bytes (hex): ";
    for (size_t i = 0; i < t.rawData.size(); ++i) {
        // Print as 2-digit hex
        unsigned char byte = static_cast<unsigned char>(t.rawData[i]);
        llvm::outs() << llvm::format_hex_no_prefix(byte, 2) << " ";
        // Limit output for massive tensors
        if (i > 16) {
            llvm::outs() << "...";
            break;
        }
    }
    llvm::outs() << " (" << t.rawData.size() << " bytes)\n";
}

void dumpValueInfo(const dlc::ValueInfo &v) {
    llvm::outs() << "   Value: " << v.name << "\n";
    llvm::outs() << "       shape: [";
    for (auto d : v.shape) llvm::outs() << (d == -1 ? "?" : std::to_string(d)) << "";
    llvm::outs() << "]\n";
}

void dumpPROTO(const onnx::ModelProto &model) {
    dlc::ModelInfo modelInfo = dlc::parseModelProto(model);

    llvm::outs() << "ONNX Model\n";
    llvm::outs() << "  IR Version: " << modelInfo.ir_version << "\n";
    llvm::outs() << "  Producer: " << modelInfo.producer_name << "\n";

    const dlc::GraphInfo &graph = modelInfo.graph;
    llvm::outs() << "  Graph: " << graph.name << "\n";
    llvm::outs() << "  Nodes: " << graph.nodes.size() << "\n";

    for (const dlc::NodeInfo &node : graph.nodes) {
        llvm::outs() << "       Op: " << node.op_type << "\n";

        llvm::outs() << "           Inputs: ";
        for (const auto &in : node.inputs)
            llvm::outs() << in << " ";
        llvm::outs() << "\n";

        llvm::outs() << "           Outputs: ";
        for (const auto &out : node.outputs)
            llvm::outs() << out << " ";
        llvm::outs() << "\n";

        // Attributes
        for (const dlc::AttributeInfo &attr : node.attributes) {
            llvm::outs() << "           Attr: " << attr.name << "\n";
            switch (attr.type) {
            case dlc::AttributeInfo::TENSOR:
                dumpTensor(attr.tensor);
                break;

            case dlc::AttributeInfo::INT:
                llvm::outs() << "               int: " << attr.i << "\n";
                break;

            case dlc::AttributeInfo::FLOAT:
                llvm::outs() << "               float: " << attr.f << "\n";
                break;

            case dlc::AttributeInfo::STRING:
                llvm::outs() << "               string: " << attr.s << "\n";
                break;

            case dlc::AttributeInfo::INTS:
                llvm::outs() << "               ints: ";
                for (auto v : attr.ints)
                    llvm::outs() << v << " ";
                llvm::outs() << "\n";
                break;

            case dlc::AttributeInfo::FLOATS:
                llvm::outs() << "               floats: ";
                for (auto v : attr.floats)
                    llvm::outs() << v << " ";
                llvm::outs() << "\n";
                break;

            default:
                llvm::outs() << "               <unknown attribute>\n";
                break;
            }
        }
    }

    // Graph outputs
    llvm::outs() << "Graph outputs: \n";
    for (const auto &out : graph.outputs) {
        dumpValueInfo(out);
    }
}

static int dumpMLIRModule(mlir::MLIRContext &context, const onnx::ModelProto &model) {
    // Parse ONNX into ModelInfo
    dlc::ModelInfo modelInfo = dlc::parseModelProto(model);

    // Generate MLIR module
    auto module = dlc::mlirGen(context, modelInfo);
    if (!module) {
        llvm::errs() << "Failed to generate MLIR module\n";
        return 1;
    }

    mlir::PassManager pm (&context);
    // If the action is DumpMLIRTensor, run the lowering pass
    if (emitAction >= DumpMLIRTensor) {

        // Lower DLC -> Tensor/Linalg
        pm.addPass(mlir::dlc::createLowerToTensorPass());
        pm.addPass(mlir::createCanonicalizerPass());
    }

    if (emitAction >= DumpMLIRMemRef) {
        // Lower Tensor/Linalg -> MemRef
        pm.addPass(mlir::bufferization::createEmptyTensorToAllocTensorPass());

        mlir::bufferization::OneShotBufferizePassOptions passOptions;

        passOptions.allowReturnAllocsFromLoops = true;
        passOptions.bufferizeFunctionBoundaries = true;

        pm.addPass(mlir::bufferization::createOneShotBufferizePass(passOptions));
        pm.addPass(mlir::bufferization::createBufferDeallocationSimplificationPass());

        pm.addPass(mlir::createConvertLinalgToLoopsPass());
        pm.addPass(mlir::createCanonicalizerPass());
        pm.addPass(mlir::createCSEPass());
    }

    if (emitAction == DumpMLIRLLVM) {
        pm.addPass(mlir::dlc::createLowerToLLVMPass());
        pm.addPass(mlir::createReconcileUnrealizedCastsPass());
    }

    // Run the pipeline if any passes were added
    if (emitAction != DumpMLIR) {
        if (mlir::failed(pm.run(*module))) {
            llvm::errs() << "Lowering pipeline failed\n";
            return 1;
        }
    }

    // Print MLIR module
    module->print(llvm::outs());
    llvm::outs() << "\n";
    return 0;
}


// MAIN
int main(int argc, char **argv) {
    cl::ParseCommandLineOptions(argc, argv, "deep learning compiler\n");
    // mlir::MLIRContext context;
    mlir::DialectRegistry registry;

    // Register dialects into registry
    registry.insert<mlir::arith::ArithDialect,
                    mlir::tensor::TensorDialect,
                    mlir::linalg::LinalgDialect,
                    mlir::memref::MemRefDialect,
                    mlir::func::FuncDialect,
                    mlir::scf::SCFDialect>();

    // Register bufferizable interface extensions with the registry
    mlir::arith::registerBufferizableOpInterfaceExternalModels(registry);
    mlir::tensor::registerBufferizableOpInterfaceExternalModels(registry);
    mlir::linalg::registerBufferizableOpInterfaceExternalModels(registry);
    mlir::bufferization::func_ext::registerBufferizableOpInterfaceExternalModels(registry);


    // Attach the registry to the context
    mlir::MLIRContext context(registry);

    context.loadDialect<mlir::cf::ControlFlowDialect,
                        mlir::arith::ArithDialect,
                        mlir::func::FuncDialect>();

    auto model = loadONNXModel(inputFilename);
    if (!model)
        return 1;

    switch (emitAction) {
    case DumpPROTO:
        dumpPROTO(*model);
        return 0;
    case DumpMLIR:
    case DumpMLIRTensor:
    case DumpMLIRMemRef:
    case DumpMLIRLLVM:
        return dumpMLIRModule(context, *model);
    default:
        llvm::errs()
            << "No action specified, use -emit=proto\n";
        return 1;
    }
}