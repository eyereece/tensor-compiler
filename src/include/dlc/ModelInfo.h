#ifndef MODEL_INFO_H
#define MODEL_INFO_H

// #include <onnx/onnx.pb.h>

#include <string>
#include <vector>
#include <cstdint>
#include <map>

namespace dlc {

struct TensorInfo {
    std::string name;
    enum class DataType { FLOAT, INT64, INT32, BOOL, UINT8, UNKNOWN };
    std::vector<int64_t> shape;
    DataType elementType;
    std::vector<char> rawData;

    bool isScalar() const {
        return shape.empty() || (shape.size() == 1 && shape[0] == 1);
    };
};

struct ValueInfo {
    std::string name;
    std::vector<int64_t> shape;
    TensorInfo::DataType elementType;
};

struct AttributeInfo {
        std::string name;
        enum Type { TENSOR, INT, FLOAT, STRING, INTS, FLOATS, UNKNOWN } type;

        // union-like storage for values
        TensorInfo tensor;
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
        std::vector<ValueInfo> inputs;
        std::vector<ValueInfo> outputs;
        std::map<std::string, TensorInfo> initializers;
};

struct ModelInfo {
    int64_t ir_version;
    std::string producer_name;
    GraphInfo graph;
};
}   // namespace dlc

#endif // MODEL_INFO_H