#include <onnx/onnx_pb.h>

#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <google/protobuf/text_format.h>

#include "llvm/ADT/StringRef.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/MemoryBuffer.h"
#include "llvm/Support/raw_ostream.h"

#include <fstream>
#include <memory>
#include <string>

namespace cl = llvm::cl;

// COMMAND-LINE OPTIONS
static cl::opt<std::string> inputFilename(
    cl::Positional,
    cl::desc("<input onnx file>"),
    cl::Required,
    cl::value_desc("filename")
);

namespace {
enum Action { None, DumpPROTO };
}

static cl::opt<enum Action>
    emitAction("emit",
                cl::desc("Select the kind of output desired"),
                cl::values(
                    clEnumValN(DumpPROTO, "proto",
                                "output the ONNX protobuf graph")
                ),
                cl::init(None));

// ONNX Loading
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

    return model;
}

// Dump IR
static void dumpPROTO(const onnx::ModelProto &model) {
    // Full model
    llvm::outs() << "=== ONNX Model Proto ===\n";
    llvm::outs() << model.DebugString() << "\n";

    // ONLY the graph
    // llvm::outs() << model.graph().DebugString() << "\n";
}

// MAIN
int main(int argc, char **argv) {
    cl::ParseCommandLineOptions(argc, argv, "deep learning compiler\n");

    auto model = loadONNXModel(inputFilename);
    if (!model)
        return 1;

    switch (emitAction) {
    case DumpPROTO:
        dumpPROTO(*model);
        return 0;
    default:
        llvm::errs()
            << "No action specified, use -emit=proto\n";
        return 1;
    }
}