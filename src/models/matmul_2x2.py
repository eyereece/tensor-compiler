import onnx
from onnx import helper, TensorProto
import numpy as np
import os

os.makedirs("onnx_files", exist_ok=True)
os.makedirs("proto_files", exist_ok=True)

# 1. INITIALIZER: Weights [2, 2]
# Instead of a vector [2], use a matrix [2, 2]
weight_val = np.array([[1.0, 2.0], [3.0, 4.0]], dtype=np.float32)
initializer_tensor = helper.make_tensor(
    name="stem.1.weight",
    data_type=TensorProto.FLOAT,
    dims=[2, 2],
    vals=weight_val.tobytes(),
    raw=True
)

# 2. CONSTANTS [2, 2]
val1 = np.array([[0.5, 0.5], [0.5, 0.5]], dtype=np.float32)
tensor1 = helper.make_tensor(
    name="const_tensor_1",
    data_type=TensorProto.FLOAT,
    dims=[2, 2],
    vals=val1.tobytes(),
    raw=True
)

# For the MatMul: [2, 2] @ [2, 2] -> [2, 2]
val2 = np.array([[2.0, 0.0], [0.0, 2.0]], dtype=np.float32) # Identity * 2
tensor2 = helper.make_tensor(
    name="const_tensor_2",
    data_type=TensorProto.FLOAT,
    dims=[2, 2],
    vals=val2.tobytes(),
    raw=True
)

# 3. NODES (Names remain the same, logic handles the rank automatically)
const_node_1 = helper.make_node("Constant", inputs=[], outputs=["const_out_1"], value=tensor1)
const_node_2 = helper.make_node("Constant", inputs=[], outputs=["const_out_2"], value=tensor2)

add_node_1 = helper.make_node("Add", inputs=["x", "stem.1.weight"], outputs=["out1"])
add_node_2 = helper.make_node("Add", inputs=["out1", "const_out_1"], outputs=["out2"])
relu_node = helper.make_node("Relu", inputs=["out2"], outputs=["out3"])
matmul_node = helper.make_node("MatMul", inputs=["out3", "const_out_2"], outputs=["final_out"])

# 4. GRAPH: Update input/output info to [2, 2]
graph = helper.make_graph(
    nodes=[const_node_1, const_node_2, add_node_1, add_node_2, relu_node, matmul_node],
    name="MatrixMatMulGraph",
    inputs=[
        helper.make_tensor_value_info("x", TensorProto.FLOAT, [2, 2])
    ],
    outputs=[
        helper.make_tensor_value_info("final_out", TensorProto.FLOAT, [2, 2])
    ],
    initializer=[initializer_tensor]
)

opset_version = helper.make_opsetid("", 11)
model = helper.make_model(graph, producer_name="dlc-compiler", ir_version=8, opset_imports=[opset_version])

onnx_path = "onnx_files/matmul_2x2.onnx"
onnx.save(model, onnx_path)

with open("proto_files/matmul_2x2.prototxt", "w") as f:
    f.write(str(model))

print(f"Generated {onnx_path} and matmul_2x2.prototxt")