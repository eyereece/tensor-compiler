import onnx
from onnx import helper, TensorProto
import numpy as np
import os

# Create directories
os.makedirs("onnx_files", exist_ok=True)
os.makedirs("proto_files", exist_ok=True)

# Sample input: x: [5.0, 5.0]

# 1. INITIALIZER: Weights "stem.1.weight"
weight_val = np.array([10.0, -20.0], dtype=np.float32)
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

# 3. NODES
# Constant Op
const_node = helper.make_node(
    "Constant",
    inputs=[],
    outputs=["const_out"],
    value=tensor1,
    name="my_constant_op"
)

# First Add: x + weights
add_node1 = helper.make_node(
    "Add",
    inputs=["x", "stem.1.weight"],
    outputs=["sum1"],
    name="add_weights"
)

# Second Add: sum1 + constant
add_node2 = helper.make_node(
    "Add",
    inputs=["sum1", "const_out"],
    outputs=["sum2"],
    name="add_const"
)

# NEW: ReLU Op
# Mathematical function: f(x) = max(0, x)
relu_node = helper.make_node(
    "Relu",
    inputs=["sum2"],
    outputs=["final_out"],
    name="relu_activation"
)

# 4. GRAPH
graph = helper.make_graph(
    nodes=[const_node, add_node1, add_node2, relu_node],
    name="ReluGraph",
    inputs=[
        helper.make_tensor_value_info("x", TensorProto.FLOAT, [2])
    ],
    outputs=[
        helper.make_tensor_value_info("final_out", TensorProto.FLOAT, [2])
    ],
    initializer=[initializer_tensor]
)

model = helper.make_model(graph, producer_name="dlc-compiler")

# Save as .onnx and .prototxt
onnx_path = "onnx_files/add_relu.onnx"
onnx.save(model, onnx_path)

with open("proto_files/add_relu.prototxt", "w") as f:
    f.write(str(model))

print(f"Generated {onnx_path} and add_relu.prototxt")