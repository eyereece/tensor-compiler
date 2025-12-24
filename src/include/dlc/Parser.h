#ifndef PARSER_H
#define PARSER_H

#include "ModelInfo.h"

// Forward declare
namespace onnx {
class ModelProto;
}

namespace dlc {
    ModelInfo parseModelProto(const onnx::ModelProto &model);
}   // namespace dlc

#endif // PARSER_H