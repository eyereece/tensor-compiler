#ifndef MODEL_INFO_H
#define MODEL_INFO_H

#include <onnx/onnx.pb.h>

#include <string>
#include <vector>
#include <cstdint>

struct AttributeInfo {
        std::string name;
        enum Type { TENSOR, INT, FLOAT, STRING, INTS, FLOATS, UNKNOWN } type;

        // union-like storage for values
        onnx::TensorProto tensor;
        int64_t i;
        float f;
        std::string s;
        std::vector<int64_t> ints;
        std::vector<float> floats;
};

struct NodeInfo {
    std::string op_type;
    std::vector<std::string> inputs;
    std::vector<std::string> outputs;
    std::vector<AttributeInfo> attributes;
};

struct GraphInfo {
        std::string name;
        std::vector<NodeInfo> nodes;
        std::vector<onnx::ValueInfoProto> inputs;
        std::vector<onnx::ValueInfoProto> outputs;
};

struct ModelInfo {
    int64_t ir_version;
    std::string producer_name;
    GraphInfo graph;
};

#endif // MODEL_INFO_H