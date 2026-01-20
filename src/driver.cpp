#include "dlc/ModelInfo.h"
#include "dlc/OnnxDumping.h"
#include "dlc/Parser.h"
#include "dlc/MLIRGen.h"
#include "dlc/Dialect.h"
#include "dlc/Passes.h"

// #include <google/protobuf/io/zero_copy_stream_impl.h>
// #include <google/protobuf/text_format.h>

#include "llvm/Support/CommandLine.h"
#include "llvm/Support/MemoryBuffer.h"

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

// TESTS
#include "mlir/Parser/Parser.h"

// #include <fstream>
#include <memory>
#include <string>
#include <ostream>

namespace cl = llvm::cl;

// COMMAND-LINE OPTIONS
static cl::list<float> inputDataList(
    "input-data",
    cl::desc("Specify input data as a space-separated list of floats"),
    cl::ZeroOrMore,
    cl::CommaSeparated
);

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
        mlir::OpPrintingFlags flags;
        flags.enableDebugInfo();
        module->print(llvm::outs(), flags);
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
static int runJit(mlir::ModuleOp module,
                    int64_t outRank,
                    const std::vector<int64_t>& inRanks,
                    const llvm::cl::list<float>& cliData) {
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

    // Prepare input data
    std::vector<std::vector<int64_t>> inputDescriptors;
    std::vector<std::vector<float>> inputDataStore; // To keep memory alive
    std::vector<int64_t*> inputPtrs;

    size_t dataOffset = 0;

    for (int64_t r : inRanks) {
        // Calculate how many elements this input needs
        // Assume fixed size 2 based on current models
        int64_t numElements = (r == 0) ? 1 : 2;

        std::vector<float> data;
        for (int64_t i = 0; i < numElements; ++i) {
            if (dataOffset < cliData.size()) {
                data.push_back(cliData[dataOffset++]);
            } else {
                data.push_back(0.0f);   // Default fallback
            }
        }
        inputDataStore.push_back(data);

        size_t slots = 3 + (2 * r);
        inputDescriptors.emplace_back(slots, 0);
        auto& desc = inputDescriptors.back();

        desc[0] = reinterpret_cast<int64_t>(inputDataStore.back().data());  // allocated
        desc[1] = desc[0];  // aligned
        desc[2] = 0;    // offset
        if (r > 0) {
            desc[3] = numElements;    // size
            desc[4] = 1;    // stride
        }
        inputPtrs.push_back(desc.data());
    }

    // Prepare output
    size_t outSlots = 3 + (2 * outRank);
    std::vector<int64_t> outDesc(outSlots, 0);
    int64_t *outPtr = outDesc.data();

    // The packed argument array
    // result first, then all inputs
    std::vector<void*> args;
    args.push_back(&outPtr);
    for (size_t i = 0; i < inputPtrs.size(); ++i) {
        args.push_back(&inputPtrs[i]);
    }

    // END TEMP

    // Invoke
    if (engine->invokePacked("_mlir_ciface_main", args)) {
        llvm::errs() << "JIT execution failed\n";
        return -1;
    }

    // Read data
    float *allocated = reinterpret_cast<float *>(outDesc[0]);
    float *aligned = reinterpret_cast<float *>(outDesc[1]);
    int64_t offset = outDesc[2];

    llvm::outs() << "Result Shape: [ ";
    int64_t totalElements = 1;  // initial number, loop will calculate actual totalElements
    for (int64_t i = 0; i < outRank; ++i) {
        int64_t s = outDesc[3 + i];
        llvm::outs() << s << " ";
        totalElements *= s;
    }
    llvm::outs() << "]\n";

    llvm::outs() << "Data: ";
    for (int64_t i = 0; i < totalElements; ++i) {
        llvm::outs() << llvm::format("%.7f ", aligned[offset + i]);
    }
    llvm::outs() << "\n";
    llvm::outs().flush();

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
                    mlir::scf::SCFDialect,
                    mlir::dlc::DlcDialect>();

    // Register bufferizable interface extensions with the registry
    mlir::arith::registerBufferizableOpInterfaceExternalModels(registry);
    mlir::tensor::registerBufferizableOpInterfaceExternalModels(registry);
    mlir::linalg::registerBufferizableOpInterfaceExternalModels(registry);
    mlir::bufferization::func_ext::registerBufferizableOpInterfaceExternalModels(registry);


    // Attach the registry to the context
    mlir::MLIRContext context(registry);

    context.loadDialect<mlir::dlc::DlcDialect,
                        mlir::cf::ControlFlowDialect,
                        mlir::arith::ArithDialect,
                        mlir::func::FuncDialect>();
    
    mlir::OwningOpRef<mlir::ModuleOp> module;
    std::unique_ptr<onnx::ModelProto> model_ptr;

    llvm::StringRef input(inputFilename);

    if (input.ends_with(".onnx")) {
        model_ptr = dlc::loadONNXModel(inputFilename);
        if (model_ptr) {
            dlc::ModelInfo modelInfo = dlc::parseModelProto(*model_ptr);
            module = dlc::mlirGen(context, modelInfo);
        }
    } else {
        module = mlir::parseSourceFile<mlir::ModuleOp>(inputFilename, &context);
    }

    if (!module) {
        llvm::errs() << "Error: Failed to load/parse input file: " << inputFilename << "\n";
        return 1;
    }

    switch (emitAction) {
    case DumpPROTO:
        if (model_ptr) {
            dlc::dumpPROTO(*model_ptr);
        } else {
            llvm::errs() << "Error: No ONNX model loaded for -emit=proto\n";
            return 1;
        }
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

        // Get input ranks
        std::vector<int64_t> inRanks;
        for (auto type : mainFunc.getArgumentTypes()) {
            auto tensorType = llvm::cast<mlir::RankedTensorType>(type);
            inRanks.push_back(tensorType.getRank());
        }

        // Get output rank
        auto outTensorType = llvm::cast<mlir::RankedTensorType>(mainFunc.getResultTypes()[0]);
        int64_t outRank = outTensorType.getRank();

        if (processMLIR(context, *module)) return 1;
        return runJit(*module, outRank, inRanks, inputDataList);
    }
    default:
        llvm::errs()
            << "No action specified, use -emit=proto\n";
        return 1;
    }
}