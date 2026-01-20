import onnx
from google.protobuf import text_format

ONNX_FILENAME = "onnx_files/mnist.onnx"
PROTOTXT_FILENAME = "proto_files/mnist.prototxt"

model = onnx.load(ONNX_FILENAME)

with open(PROTOTXT_FILENAME, "w") as f:
    f.write(text_format.MessageToString(model))

print("Converted ONNX file to prototxt")