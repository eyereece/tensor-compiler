#include "dlc/ModelInfo.h"
#include "dlc/OnnxDumping.h"
#include "dlc/Parser.h"
#include "dlc/MLIRGen.h"
#include "dlc/Dialect.h"
#include "dlc/Passes.h"

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
#include "mlir/Dialect/SCF/Transforms/BufferizableOpInterfaceImpl.h"
#include "mlir/Dialect/Arith/IR/ValueBoundsOpInterfaceImpl.h"
#include "mlir/Dialect/SCF/IR/ValueBoundsOpInterfaceImpl.h"
#include "mlir/Dialect/Tensor/IR/ValueBoundsOpInterfaceImpl.h"

// Dump MLIR-LLVM
#include "mlir/Conversion/Passes.h"
#include "mlir/Conversion/ReconcileUnrealizedCasts/ReconcileUnrealizedCasts.h"
#include "mlir/Dialect/LLVMIR/LLVMDialect.h"
#include "mlir/Dialect/ControlFlow/IR/ControlFlow.h"
#include "mlir/Dialect/MemRef/Transforms/Passes.h"

// Dump LLVM IR
#include "mlir/Target/LLVMIR/Dialect/Builtin/BuiltinToLLVMIRTranslation.h"
#include "mlir/Target/LLVMIR/Dialect/LLVMIR/LLVMToLLVMIRTranslation.h"
#include "mlir/Target/LLVMIR/Export.h"
#include "mlir/ExecutionEngine/OptUtils.h"
#include "mlir/ExecutionEngine/ExecutionEngine.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/ExecutionEngine/Orc/JITTargetMachineBuilder.h"
#include "mlir/Conversion/AffineToStandard/AffineToStandard.h"

// JIT
#include "mlir/IR/BuiltinTypes.h"

// TESTS
#include "mlir/Parser/Parser.h"

#include <memory>
#include <string>
#include <ostream>
#include <chrono>
#include <random>
#include <fstream>

namespace cl = llvm::cl;

// COMMAND-LINE OPTIONS
static cl::list<float> inputDataList(
    "input-data",
    cl::desc("Specify input data as a space-separated list of floats"),
    cl::ZeroOrMore,
    cl::CommaSeparated
);

static cl::list<std::string> inputFiles(
    "input-files",
    cl::desc("Specify input binary files as a comma-separated list of paths"),
    cl::ZeroOrMore,
    cl::CommaSeparated
);

static cl::opt<std::string> inputFilename(
    cl::Positional,
    cl::desc("<input onnx file>"),
    cl::Required,
    cl::value_desc("filename")
);

static cl::opt<bool> runTiling(
    "tiling",
    cl::desc("Enable tiling optimization"),
    cl::init(false)
);

static cl::opt<bool> runTranspose(
    "transpose",
    cl::desc("Enable B-transpose optimization"),
    cl::init(false)
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

// Helper function to prepare data
static std::vector<std::vector<float>> prepareInputData(
    const std::vector<std::vector<int64_t>>& inShapes,
    const llvm::cl::list<float>& cliData,
    const llvm::cl::list<std::string>& cliFiles) {

        size_t totalExpected = 0;
        for (const auto& shape : inShapes) {
            int64_t prod = 1;
            for (int64_t d : shape) prod *= d;
            totalExpected += prod;
        }

        // Warning if input != expected
        if (!cliData.empty() && cliData.size() != totalExpected) {
            llvm::errs() << "Warning: provided " << cliData.size()
                        << " elements, but the model expects " << totalExpected << ".\n";
        }

        std::vector<std::vector<float>> inputDataStore;
        size_t fileIdx = 0;
        size_t cliOffset = 0;

        // Fixed seed for fallback
        std::mt19937 gen(42);
        std::uniform_real_distribution<float> dist(0.0f, 1.0f);

        for (const auto& shape : inShapes) {
            int64_t numElements = 1;
            for (int64_t dim : shape) numElements *= dim;
            std::vector<float> data(numElements);

            if (fileIdx < cliFiles.size()) {
                // Load from binary file
                std::ifstream ifs(cliFiles[fileIdx++], std::ios::binary);
                if (!ifs) {
                    llvm::errs() << "Error: Could not open input file" << cliFiles[fileIdx-1] << "\n";
                    exit(1);
                }
                ifs.read(reinterpret_cast<char*>(data.data()), numElements * sizeof(float));
            } else if (cliOffset < cliData.size()) {
                // Load from CLI
                for (int64_t j = 0; j < numElements; ++j) {
                    if (cliOffset < cliData.size()) {
                        data[j] = cliData[cliOffset++];
                    } else {
                        data[j] = 0.0f;
                    }
                }
            } else {
                // Random data (for quick benchmarks)
                for (int64_t j = 0; j < numElements; ++j) {
                    data[j] = dist(gen);
                }
            }
            inputDataStore.push_back(std::move(data));
        }
        return inputDataStore;
}

static int processMLIR(mlir::MLIRContext &context, mlir::ModuleOp module) {
    mlir::PassManager pm (&context);
    // If the action is DumpMLIRTensor, run the lowering pass
    if (emitAction >= DumpMLIRTensor) {
        // Lower DLC -> Tensor/Linalg
        pm.addPass(mlir::dlc::createLowerToTensorPass(runTranspose));

        if (runTiling) {
            pm.addPass(mlir::dlc::createLinalgTilingPass());
        }
        pm.addPass(mlir::createCanonicalizerPass());
        pm.addPass(mlir::createCSEPass());
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
        pm.addPass(mlir::memref::createExpandStridedMetadataPass());
        pm.addPass(mlir::createLowerAffinePass());
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
                    const std::vector<int64_t>& outShape,
                    const std::vector<std::vector<int64_t>>& inShapes,
                    std::vector<std::vector<float>>& inputDataStore) {
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
    engineOptions.jitCodeGenOptLevel = llvm::CodeGenOptLevel::Aggressive;
    engineOptions.sharedLibPaths = {
        "/workspace/llvm-project/build/lib/libmlir_runner_utils.so",
        "/workspace/llvm-project/build/lib/libmlir_c_runner_utils.so"
    };

    auto maybeEngine = mlir::ExecutionEngine::create(module, engineOptions);
    if (!maybeEngine) {
        llvm::errs() << "Failed to construct execution engine\n";
        return -1;
    }
    auto &engine = maybeEngine.get();

    // Prepare input data
    std::vector<std::vector<int64_t>> inputDescriptors;
    inputDescriptors.reserve(inRanks.size());

    size_t dataOffset = 0;

    // Dynamic Shape Handling for Inputs
    for (size_t i = 0; i < inRanks.size(); ++i) {
        int64_t rank = inRanks[i];
        const auto& shape = inShapes[i];

        // Build the MemRef Descriptor
        size_t slots = 3 + (2 * rank);
        inputDescriptors.emplace_back(slots, 0);
        auto& desc = inputDescriptors.back();

        desc[0] = reinterpret_cast<int64_t>(inputDataStore[i].data());  // Allocated
        desc[1] = desc[0];
        desc[2] = 0;

        // Fill Sizes and strides
        for (int64_t j = 0; j < rank; ++j) {
            desc[3 + j] = shape[j]; // Size

            // Calculate stride (assuming contiguous row-major)
            int64_t stride = 1;
            for (int64_t k = j + 1; k < rank; ++k) {
                stride *= shape[k];
            }
            desc[3 + rank + j] = stride;    // Stride
        }
    }

    // Prepare output
    int64_t totalOutElements = 1;
    for (int64_t dim : outShape) totalOutElements *= dim;
    float* outData = (float*)malloc(totalOutElements * sizeof(float));

    // Prepare output descriptor
    size_t outSlots = 3 + (2 * outRank);
    std::vector<int64_t> outDesc(outSlots, 0);

    outDesc[0] = reinterpret_cast<int64_t>(outData);    // Allocated
    outDesc[1] = reinterpret_cast<int64_t>(outData);    // Aligned
    outDesc[2] = 0;

    // Fill Sizes and strides for the output
    for (int64_t i = 0; i < outRank; ++i) {
        outDesc[3 + i] = outShape[i];

        // Calculate stride
        int64_t stride = 1;
        for (int64_t k = i + 1; k < outRank; ++k) {
            stride *= outShape[k];
        }
        outDesc[3 + outRank + i] = stride;
    }

    // The packed argument array
    // result first, then all inputs
    std::vector<void*> args;
    void* outDescPtr = outDesc.data();
    args.push_back(&outDescPtr);

    // Input Pointers (pointers to each descriptor array)
    // Use a separate vector to store these pointers to ensure
    // their addresses(&inputPtrs[i]) are stable
    std::vector<int64_t*> inputPtrs;
    for (auto& desc : inputDescriptors) {
        inputPtrs.push_back(desc.data());
    }

    for (size_t i = 0; i < inputPtrs.size(); ++i) {
        args.push_back(&inputPtrs[i]);
    }

    auto start_exec = std::chrono::high_resolution_clock::now();

    // Invoke
    if (engine->invokePacked("_mlir_ciface_main", args)) {
        llvm::errs() << "JIT execution failed\n";
        return -1;
    }
    auto end_exec = std::chrono::high_resolution_clock::now();
    
    auto start_io = std::chrono::high_resolution_clock::now();
    // Read data
    float *allocated = reinterpret_cast<float *>(outDesc[0]);
    float *aligned = reinterpret_cast<float *>(outDesc[1]);
    std::ofstream ofs("output.bin", std::ios::binary);
    if (ofs) {
        ofs.write(reinterpret_cast<char*>(aligned), totalOutElements * sizeof(float));
        ofs.close();
        llvm::outs() << "Result saved to output.bin\n";
    }
    auto end_io = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double, std::milli> exec_dur = end_exec - start_exec;
    std::chrono::duration<double, std::milli> io_dur = end_io - start_io;

    llvm::outs() << "Actual Math Time: " << exec_dur.count() << " ms\n";
    llvm::outs() << "Disk Write Time: " << io_dur.count() << " ms\n";

    int64_t offset = outDesc[2];

    // Helper to print nested dimensions
    auto printRef = [&](auto self, int64_t dim, int64_t currentOffset) -> void {
        if (dim == outRank) {
            llvm::outs() << llvm::format("%.7f", aligned[currentOffset]);
            return;
        }

        llvm::outs() << "[";
        int64_t size = outDesc[3 + dim];
        int64_t stride = outDesc[3 + outRank + dim];

        for (int64_t i = 0; i < size; ++i) {
            self(self, dim + 1, currentOffset + i * stride);
            if (i < size - 1) llvm::outs() << ", ";
        }
        llvm::outs() << "]";
    };

    llvm::outs() << "Result Shape: [ ";
    for (int64_t i = 0; i < outRank; ++i) {
        int64_t s = outDesc[3 + i];
        llvm::outs() << s << " ";
    }
    llvm::outs() << "]\n";

    // Only print data if less than 100 elements
    if (totalOutElements <= 100) {
        llvm::outs() << "Data: ";
        printRef(printRef, 0, offset);
        llvm::outs() << "\n";
    } else {
        llvm::outs() << "Data: [Large Tensor - Printing skipped for performance]\n";
        llvm::outs() << "First Element: " << aligned[offset] << "\n";
    }
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
    mlir::scf::registerBufferizableOpInterfaceExternalModels(registry);
    mlir::bufferization::func_ext::registerBufferizableOpInterfaceExternalModels(registry);

    // Register Value Bounds Op Interface
    mlir::arith::registerValueBoundsOpInterfaceExternalModels(registry);
    mlir::scf::registerValueBoundsOpInterfaceExternalModels(registry);
    mlir::tensor::registerValueBoundsOpInterfaceExternalModels(registry);


    // Attach the registry to the context
    mlir::MLIRContext context(registry);

    context.loadDialect<mlir::dlc::DlcDialect,
                        mlir::cf::ControlFlowDialect,
                        mlir::arith::ArithDialect,
                        mlir::func::FuncDialect,
                        mlir::scf::SCFDialect>();
    
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

        // Get input ranks and shapes
        std::vector<int64_t> inRanks;
        std::vector<std::vector<int64_t>> inShapes;
        for (auto type : mainFunc.getArgumentTypes()) {
            auto tensorType = llvm::cast<mlir::RankedTensorType>(type);
            inRanks.push_back(tensorType.getRank());
            inShapes.push_back(tensorType.getShape().vec());
        }

        // Get output rank and shape
        auto outTensorType = llvm::cast<mlir::RankedTensorType>(mainFunc.getResultTypes()[0]);
        int64_t outRank = outTensorType.getRank();
        std::vector<int64_t> outShape = outTensorType.getShape().vec();

        // Prepare data
        auto inputDataStore = prepareInputData(inShapes, inputDataList, inputFiles);

        if (processMLIR(context, *module)) return 1;
        return runJit(*module, outRank, inRanks, outShape, inShapes, inputDataStore);
    }
    default:
        llvm::errs()
            << "No action specified, use -emit=proto\n";
        return 1;
    }
}