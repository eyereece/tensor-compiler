#include <onnx/onnx.pb.h>

#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <google/protobuf/text_format.h>

#include "llvm/ADT/StringRef.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/MemoryBuffer.h"
#include "llvm/Support/raw_ostream.h"

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
    llvm::errs() << "Model has " << model->graph().node_size() << " nodes\n";
    return model;
}

// Init Dump IR
// static void dumpPROTO(const onnx::ModelProto &model) {
//     std::string buffer;

//     // Fill the buffer
//     if (!model.SerializeToString(&buffer)) {
//         llvm::errs() << "Failed to serialize ONNX model\n";
//     return;
//     }

//     // LLVM-style output: explicit bytes
//     llvm::outs() << "ONNX binary size = " << buffer.size() << " bytes\n";
//     llvm::outs().write(buffer.data(), buffer.size());
//     llvm::outs() << "\n";
// }

static const char *onnxDataTypeToString(int dt);

void dumpValueInfo(const onnx::ValueInfoProto &v) {
  llvm::outs() << "  Value: " << v.name() << "\n";

  if (!v.has_type())
    return;

  const auto &type = v.type();
  if (!type.has_tensor_type())
    return;

  const auto &tensor = type.tensor_type();

  // Element type
  llvm::outs() << "    dtype: "
               << onnxDataTypeToString(tensor.elem_type()) << "\n";

  // Shape
  llvm::outs() << "    shape: [ ";
  for (const auto &dim : tensor.shape().dim()) {
    if (dim.has_dim_value())
      llvm::outs() << dim.dim_value() << " ";
    else
      llvm::outs() << "? ";
  }
  llvm::outs() << "]\n";
}


static const char *onnxDataTypeToString(int dt) {
    switch (dt)
    {
    case onnx::TensorProto::FLOAT: return "float32";
    case onnx::TensorProto::DOUBLE: return "float64";
    case onnx::TensorProto::INT64: return "int64";
    case onnx::TensorProto::INT32: return "int32";
    default: return "unknown";
    }
}

// Dump tensor values
void dumpTensor(const onnx::TensorProto &t) {
    llvm::outs() << "        Tensor\n";

    // Shape
    llvm::outs() << "        shape: [ ";
    for (auto d : t.dims())
        llvm::outs() << d << " ";
    llvm::outs() << "]\n";

    // Data type
    llvm::outs() << "        dtype: "
                 << onnxDataTypeToString(t.data_type()) << "\n";

    // Values (handle common cases)
    if (t.data_type() == onnx::TensorProto::FLOAT) {
        llvm::outs() << "        values: ";
        for (float v : t.float_data())
            llvm::outs() << v << " ";
        llvm::outs() << "\n";
    } else if (t.data_type() == onnx::TensorProto::INT64) {
        llvm::outs() << "        values: ";
        for (auto v : t.int64_data())
            llvm::outs() << v << " ";
        llvm::outs() << "\n";
    }
}

void dumpPROTO(const onnx::ModelProto &model) {
    llvm::outs() << "ONNX Model\n";
    llvm::outs() << "  IR version: " << model.ir_version() << "\n";
    llvm::outs() << "  Producer:   " << model.producer_name() << "\n";

    const auto &graph = model.graph();
    llvm::outs() << "  Graph: " << graph.name() << "\n";
    llvm::outs() << "  Nodes: " << graph.node_size() << "\n";

    for (const auto &node : graph.node()) {
        llvm::outs() << "    Op: " << node.op_type() << "\n";
        llvm::outs() << "      Inputs: ";
        for (const auto &in : node.input())
            llvm::outs() << in << " ";
        llvm::outs() << "\n";

        llvm::outs() << "      Outputs: ";
        for (const auto &out : node.output())
            llvm::outs() << out << " ";
        llvm::outs() << "\n";

        // Attributes
        for (const auto &attr : node.attribute()) {
            llvm::outs() << "      Attr: " << attr.name() << "\n";

            if (attr.type() == onnx::AttributeProto::TENSOR) {
                dumpTensor(attr.t());
            } else if (attr.type() == onnx::AttributeProto::INT) {
                llvm::outs() << "      Attr int: " << attr.i() << "\n";
            } else if (attr.type() == onnx::AttributeProto::FLOAT) {
                llvm::outs() << "      Attr float: " << attr.f() << "\n";
            } else if (attr.type() == onnx::AttributeProto::STRING) {
                llvm::outs() << "      Attr string: " << attr.s() << "\n";
            } else if (attr.type() == onnx::AttributeProto::INTS) {
                llvm::outs() << "      Attr ints: ";
                for (auto v : attr.ints()) llvm::outs() << v << " ";
                llvm::outs() << "\n";
            } else if (attr.type() == onnx::AttributeProto::FLOATS) {
                llvm::outs() << "      Attr floats: ";
                for (auto v : attr.floats()) llvm::outs() << v << " ";
                llvm::outs() << "\n";
            }
        }
    }
      // Graph
    llvm::outs() << "Graph outputs:\n";
    for (const auto &out : graph.output()) {
        dumpValueInfo(out);
    }
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