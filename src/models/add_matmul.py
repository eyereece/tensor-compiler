import onnx
from onnx import helper, TensorProto
import numpy as np
import os

# Create directories
os.makedirs("onnx_files", exist_ok=True)
os.makedirs("proto_files", exist_ok=True)

# Sample input: x : [5.0, 5.0]

# 1. INITIALIZER: Weights "stem.1.weight"
weight_val = np.array([5.0, -10.0], dtype=np.float32)
initializer_tensor = helper.make_tensor(
    name="stem.1.weight",
    data_type=TensorProto.FLOAT,
    dims=[2],
    vals=weight_val.tobytes(),
    raw=True
)

# 2. CONSTANT NODES
val1 = np.array([1.0, 1.0], dtype=np.float32)
tensor1 = helper.make_tensor(
    name="const_tensor_1",
    data_type=TensorProto.FLOAT,
    dims=[2],
    vals=val1.tobytes(),
    raw=True
)

val2 = np.array([[3.0], [3.0]], dtype=np.float32)
tensor2 = helper.make_tensor(
    name="const_tensor_2",
    data_type=TensorProto.FLOAT,
    dims=[2, 1],
    vals=val2.tobytes(),
    raw=True
)

# 3. NODES
# Constant Op
const_node_1 = helper.make_node(
    "Constant",
    inputs=[],
    outputs=["const_out_1"],
    value=tensor1,
    name="constant_op_1"
)

const_node_2 = helper.make_node(
    "Constant",
    inputs=[],
    outputs=["const_out_2"],
    value=tensor2,
    name="constant_op_2"
)

# First Add: x + weights
add_node_1 = helper.make_node(
    "Add",
    inputs=["x", "stem.1.weight"],
    outputs=["out1"],
    name="add_weights"
)

# Second Add: out1 + constant
add_node_2 = helper.make_node(
    "Add",
    inputs=["out1", "const_out_1"],
    outputs=["out2"],
    name="add_const"
)

# Third: relu Op
relu_node = helper.make_node(
    "Relu",
    inputs=["out2"],
    outputs=["out3"],
    name="relu_activation"
)

# NEW: MatMul Op
matmul_node = helper.make_node(
    "MatMul",
    inputs=["out3", "const_out_2"],
    outputs=["final_out"],
    name="matmul_node"
)

# 4. GRAPH
graph = helper.make_graph(
    nodes=[const_node_1, const_node_2, add_node_1, add_node_2, relu_node, matmul_node],
    name="MatMulGraph",
    inputs=[
        helper.make_tensor_value_info("x", TensorProto.FLOAT, [2])
    ],
    outputs=[
        helper.make_tensor_value_info("final_out", TensorProto.FLOAT, [1])
    ],
    initializer=[initializer_tensor]
)

opset_version = helper.make_opsetid("", 23)
model = helper.make_model(graph,
                        producer_name="dlc-compiler",
                        ir_version=8,
                        opset_imports=[opset_version])

# Save model
onnx_path = "onnx_files/add_matmul.onnx"
onnx.save(model, onnx_path)

with open("proto_files/add_matmul.prototxt", "w") as f:
    f.write(str(model))

print(f"Generated {onnx_path} and add_matmul.prototxt")