#include "dlc/Parser.h"
#include "dlc/ModelInfo.h"

#include <onnx/onnx.pb.h>

namespace dlc {

static AttributeInfo parseAttribute(const onnx::AttributeProto &attr) {
    AttributeInfo info;
    info.name = attr.name();
    switch (attr.type()) {
    case onnx::AttributeProto::TENSOR:
        info.type = AttributeInfo::TENSOR;
        info.tensor = attr.t();
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
        g.inputs.push_back(in);
    for (const auto &out : graph.output())
        g.outputs.push_back(out);
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