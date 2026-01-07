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

// Bufferization
#include "mlir/Dialect/Bufferization/Transforms/OneShotAnalysis.h"
#include "mlir/Dialect/Bufferization/Transforms/OneShotModuleBufferize.h"
#include "mlir/Dialect/Bufferization/Transforms/Passes.h"
#include "mlir/Dialect/MemRef/IR/MemRef.h"
#include "mlir/Dialect/Bufferization/IR/BufferizableOpInterface.h"
#include "mlir/Dialect/Arith/Transforms/BufferizableOpInterfaceImpl.h"
#include "mlir/Dialect/Tensor/Transforms/BufferizableOpInterfaceImpl.h"
#include "mlir/Dialect/Linalg/Transforms/BufferizableOpInterfaceImpl.h"
#include "mlir/Dialect/Bufferization/Transforms/FuncBufferizableOpInterfaceImpl.h"

// Dump MLIR-LLVM
#include "mlir/Conversion/Passes.h"
#include "mlir/Conversion/ReconcileUnrealizedCasts/ReconcileUnrealizedCasts.h"
#include "mlir/Dialect/LLVMIR/LLVMDialect.h"
#include "mlir/Dialect/ControlFlow/IR/ControlFlow.h"

// Dump LLVM IR
#include "mlir/Target/LLVMIR/Dialect/Builtin/BuiltinToLLVMIRTranslation.h"
#include "mlir/Target/LLVMIR/Dialect/LLVMIR/LLVMToLLVMIRTranslation.h"
#include "mlir/Target/LLVMIR/Export.h"
#include "mlir/Transforms/Passes.h"
#include "mlir/ExecutionEngine/OptUtils.h"
#include "mlir/ExecutionEngine/ExecutionEngine.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/ExecutionEngine/Orc/JITTargetMachineBuilder.h"

// JIT
#include "mlir/IR/BuiltinTypes.h"

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
enum Action { 
            None,
            DumpPROTO,
            DumpMLIR,
            DumpMLIRTensor,
            DumpMLIRMemRef,
            DumpMLIRLLVM,
            DumpLLVMIR,
            RunJIT
         };
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
                                "output the MLIR-LLVM dump after llvm lowering"),
                    clEnumValN(DumpLLVMIR, "llvm",
                                "output the LLVM IR dump"),
                    clEnumValN(RunJIT, "jit",
                                "JIT the code and run it")
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

static int processMLIR(mlir::MLIRContext &context, mlir::ModuleOp module) {
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

    if (emitAction >= DumpMLIRLLVM || emitAction == DumpLLVMIR) {
        pm.addPass(mlir::dlc::createLowerToLLVMPass());
        pm.addPass(mlir::createReconcileUnrealizedCastsPass());
    }

    // Run the pipeline if any passes were added

    if (mlir::failed(pm.run(module))) {
        llvm::errs() << "Lowering pipeline failed\n";
        return 1;
    }


    // Print MLIR module
    if (emitAction <= DumpMLIRLLVM) {
        module->print(llvm::outs());
        llvm::outs() << "\n";
    }
    return 0;
}

static int dumpLLVMIR(mlir::ModuleOp module) {
    // Register translations to LLVM IR
    mlir::registerBuiltinDialectTranslation(*module->getContext());
    mlir::registerLLVMDialectTranslation(*module->getContext());

    // Convert MLIR Module to LLVM IR module
    llvm::LLVMContext llvmContext;
    auto llvmModule = mlir::translateModuleToLLVMIR(module, llvmContext);
    if (!llvmModule) {
        llvm::errs() << "Failed to emit LLVM IR\n";
        return -1;
    }

    // Initialize LLVM targets (needed for DataLayout)
    llvm::InitializeNativeTarget();
    llvm::InitializeNativeTargetAsmPrinter();

    // Set up Data Layout and Target Triple
    auto tmBuilderOrError = llvm::orc::JITTargetMachineBuilder::detectHost();
    if (!tmBuilderOrError) {
        llvm::errs() << "Could not create JITTargetMachineBuilder\n";
        return -1;
    }
    auto tmOrError = tmBuilderOrError->createTargetMachine();
    if (!tmOrError) {
        llvm::errs() << "Could not create TargetMachine\n";
        return -1;
    }

    mlir::ExecutionEngine::setupTargetTripleAndDataLayout(
        llvmModule.get(), tmOrError.get().get()
    );

    // Output the result
    llvm::outs() << *llvmModule << "\n";
    return 0;
}

// Run JIT
static int runJit(mlir::ModuleOp module, int64_t rank) {
    // Initialize LLVM targets for the host machine
    llvm::InitializeNativeTarget();
    llvm::InitializeNativeTargetAsmPrinter();

    // Register translations to LLVM IR
    mlir::registerBuiltinDialectTranslation(*module->getContext());
    mlir::registerLLVMDialectTranslation(*module->getContext());

    // Create an MLIR execution engine
    // optLevel 2 or 3 enables LLVM optimization
    auto optPipeline = mlir::makeOptimizingTransformer(3, 0, nullptr);
    mlir::ExecutionEngineOptions engineOptions;
    engineOptions.transformer = optPipeline;

    auto maybeEngine = mlir::ExecutionEngine::create(module, engineOptions);
    if (!maybeEngine) {
        llvm::errs() << "Failed to construct execution engine\n";
        return -1;
    }
    auto &engine = maybeEngine.get();

    // Calculate how many 64-bit slots needed: 3 + rank + rank
    size_t numSlots = 3 + (2 * rank);
    std::vector<int64_t> descriptor(numSlots, 0);
    
    // Pass a pointer to the start of the vector's data
    int64_t *descriptorPtr = descriptor.data();

    // Pass the address of the pointer to the data
    void *args[] = { &descriptorPtr };

    if (engine->invokePacked("_mlir_ciface_main", args)) {
        llvm::errs() << "JIT execution failed\n";
        return -1;
    }

    // descriptor is now filled with data
    float *allocated = reinterpret_cast<float *>(descriptor[0]);
    float *aligned = reinterpret_cast<float *>(descriptor[1]);
    int64_t offset = descriptor[2];

    // Read sizes (starts at index 3)
    llvm::outs() << "Result Shape: [ ";
    int64_t totalElements = (rank == 0) ? 1 : 1;
    for(int64_t i = 0; i < rank; ++i) {
        int64_t s = descriptor[3 + i];
        llvm::outs() << s << " ";
        totalElements *= s;
    }
    llvm::outs() << "]\n";

    // Print the data
    llvm::outs() << "Data: ";
    for (int64_t i = 0; i < totalElements; ++i) {
        llvm::outs() << aligned[offset + i] << " ";
    }
    llvm::outs() << "\n";

    // Clean up
    if (allocated && (uintptr_t)allocated != 0xDEADBEEF) free(allocated);

    return 0;
}


// MAIN
int main(int argc, char **argv) {
    // Register any command line options
    mlir::registerAsmPrinterCLOptions();
    mlir::registerMLIRContextCLOptions();
    mlir::registerPassManagerCLOptions();

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

    dlc::ModelInfo modelInfo = dlc::parseModelProto(*model);
    auto module = dlc::mlirGen(context, modelInfo);

    switch (emitAction) {
    case DumpPROTO:
        dumpPROTO(*model);
        return 0;
    case DumpMLIR:
    case DumpMLIRTensor:
    case DumpMLIRMemRef:
    case DumpMLIRLLVM:
        return processMLIR(context, *module);
    case DumpLLVMIR:
        if (processMLIR(context, *module)) return 1;
        return dumpLLVMIR(*module);
    case RunJIT: {
        // Find the main function created by mlirGen
        auto mainFunc = module->lookupSymbol<mlir::func::FuncOp>("main");
        if (!mainFunc) {
            llvm::errs() << "Error: main function not found after mlirGen\n";
            return 1;
        }

        // Get the rank from the TENSOR type
        auto tensorType = llvm::cast<mlir::RankedTensorType>(mainFunc.getResultTypes()[0]);
        int64_t rank = tensorType.getRank();
        if (processMLIR(context, *module)) return 1;
        return runJit(*module, rank);
    }
    default:
        llvm::errs()
            << "No action specified, use -emit=proto\n";
        return 1;
    }
}