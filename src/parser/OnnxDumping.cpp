#include "dlc/OnnxDumping.h"
#include "dlc/Parser.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/Format.h"
#include <fstream>
#include <memory>

namespace dlc {
std::unique_ptr<onnx::ModelProto>
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

    // INITIALIZERS
    llvm::outs() << "  Initializers [" << graph.initializers.size() << "]\n";
    for (const auto &pair : graph.initializers) {
        const std::string &name = pair.first;
        const dlc::TensorInfo &tensor = pair.second;

        llvm::outs() << "    Name: " << name << "\n";
        dumpTensor(tensor);
        llvm::outs() << "\n";
    }
    // END INITIALIZERS

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

}   // namespace dlc