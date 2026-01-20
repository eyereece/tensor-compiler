#ifndef DLC_ONNXDUMPING_H
#define DLC_ONNXDUMPING_H

#include <onnx/onnx.pb.h>
#include "dlc/ModelInfo.h"
#include "llvm/ADT/StringRef.h"
#include <memory>

namespace dlc {
std::unique_ptr<onnx::ModelProto> loadONNXModel(llvm::StringRef filename);
void dumpPROTO(const onnx::ModelProto &model);

void dumpTensor(const dlc::TensorInfo &t);
void dumpValueInfo(const dlc::ValueInfo &v);
}   // namespace dlc

#endif // DLC_ONNXDUMPING_H