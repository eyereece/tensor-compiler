#ifndef PARSER_H
#define PARSER_H

#include "ModelInfo.h"

// Forward declare
namespace onnx {
class ModelProto;
}

ModelInfo parseModelProto(const onnx::ModelProto &model);

#endif // PARSER_H