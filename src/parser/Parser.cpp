#include "dlc/Parser.h"
#include "dlc/ModelInfo.h"

#include <onnx/onnx.pb.h>

namespace dlc {

static TensorInfo parseTensor(const onnx::TensorProto &t) {
    TensorInfo info;

    if (!t.name().empty()) {
        info.name = t.name();
    }
    
    // Extract shape
    for (int64_t d : t.dims()) {
        info.shape.push_back(d);
    }

    // Map ONNX data type to Enum data type
    switch(t.data_type()) {
        case 1: info.elementType = TensorInfo::DataType::FLOAT; break;
        case 6: info.elementType = TensorInfo::DataType::INT32; break;
        case 7: info.elementType = TensorInfo::DataType::INT64; break;
        default: 
            info.elementType = TensorInfo::DataType::UNKNOWN;
            std::cerr << "Warning: Unsupported ONNX data type: " << t.data_type() << std::endl;
            break;
    }

    // Raw data logic
    if (t.has_raw_data()) {
        // copy the bytes directly
        const std::string &bytes = t.raw_data();
        info.rawData.assign(bytes.begin(), bytes.end());
    } else if (t.float_data_size() > 0) {
        // If onnx used the repeated float field, pack it into bytes
        size_t byteSize = t.float_data_size() * sizeof(float);
        info.rawData.resize(byteSize);
        std::memcpy(info.rawData.data(), t.float_data().data(), byteSize);
    }

    return info;

}

static ValueInfo parseValueInfo(const onnx::ValueInfoProto &v) {
    ValueInfo info;
    info.name = v.name();

    if (v.has_type() && v.type().has_tensor_type()) {
        const auto &tensorType = v.type().tensor_type();

        // Map element type
        switch (tensorType.elem_type()) {
            case 1: info.elementType = TensorInfo::DataType::FLOAT; break;
            case 7: info.elementType = TensorInfo::DataType::INT64; break;
            default: info.elementType = TensorInfo::DataType::UNKNOWN; break;
        }

        // Map shape
        for (const auto &dim : tensorType.shape().dim()) {
            if (dim.has_dim_value()) {
                info.shape.push_back(dim.dim_value());
            } else {
                // Handle dynamic shapes
                info.shape.push_back(-1);
            }
        }
    }
    return info;
}

static AttributeInfo parseAttribute(const onnx::AttributeProto &attr) {
    AttributeInfo info;
    info.name = attr.name();
    switch (attr.type()) {
    case onnx::AttributeProto::TENSOR:
        info.type = AttributeInfo::TENSOR;
        info.tensor = parseTensor(attr.t());
        break;

    case onnx::AttributeProto::INT:
        info.type = AttributeInfo::INT;
        info.i = attr.i();
        break;

    case onnx::AttributeProto::FLOAT:
        info.type = AttributeInfo::FLOAT;
        info.f = attr.f();
        break;

    case onnx::AttributeProto::STRING:
        info.type = AttributeInfo::STRING;
        info.s = attr.s();
        break;

    case onnx::AttributeProto::INTS:
        info.type = AttributeInfo::INTS;
        info.ints.assign(attr.ints().begin(), attr.ints().end());
        break;

    case onnx::AttributeProto::FLOATS:
        info.type = AttributeInfo::FLOATS;
        info.floats.assign(attr.floats().begin(), attr.floats().end());
        break;
    }

    return info;
}

static NodeInfo parseNode(const onnx::NodeProto &node) {
    NodeInfo n;
    n.op_type = node.op_type();
    
    for (const auto &in : node.input())
        n.inputs.push_back(in);

    for (const auto &out : node.output())
        n.outputs.push_back(out);

    for (const auto &attr : node.attribute())
        n.attributes.push_back(parseAttribute(attr));
    
    return n;
}

static GraphInfo parseGraph(const onnx::GraphProto &graph) {
    GraphInfo g;
    g.name = graph.name();
    
    for (const auto &node : graph.node())
        g.nodes.push_back(parseNode(node));
    for (const auto &in : graph.input())
        g.inputs.push_back(parseValueInfo(in));
    for (const auto &out : graph.output())
        g.outputs.push_back(parseValueInfo(out));
    return g;
}

ModelInfo parseModelProto(const onnx::ModelProto &model) {
    ModelInfo info;
    info.ir_version = model.ir_version();
    info.producer_name = model.producer_name();
    info.graph = parseGraph(model.graph());
    return info;
}

}   // namespace dlc