import onnx
from onnx import helper, TensorProto
import numpy as np
import os

os.makedirs("onnx_files", exist_ok=True)

# 1. INITIALIZER: This represents weights "stem.1.weight"
weight_val = np.array([10.0, 20.0], dtype=np.float32)
initializer_tensor = helper.make_tensor(
    name="stem.1.weight",
    data_type=TensorProto.FLOAT,
    dims=[2],
    vals=weight_val.tobytes(),
    raw=True
)

# 2. CONSTANT NODES: Internal hardcoded values
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

# First Add: Input 'x' + Initializer 'stem.1.weight'
add_node1 = helper.make_node(
    "Add",
    inputs=["x", "stem.1.weight"],
    outputs=["sum1"],
    name="add_weights"
)

# Second Add: sum1 + Constant 'const_out'
add_node2 = helper.make_node(
    "Add",
    inputs=["sum1", "const_out"],
    outputs=["final_out"],
    name="add_const"
)

# 4. GRAPH
graph = helper.make_graph(
    nodes=[const_node, add_node1, add_node2],
    name="ToyCompilerGraph",
    inputs=[
        # Function argument 'x'
        helper.make_tensor_value_info("x", TensorProto.FLOAT, [2])
    ],
    outputs=[
        helper.make_tensor_value_info("final_out", TensorProto.FLOAT, [2])
    ],
    initializer=[initializer_tensor] # Weights
)

model = helper.make_model(graph, producer_name="toy-compiler")

# Save as .onnx and .prototxt
onnx_path = "onnx_files/add_const_init.onnx"
onnx.save(model, onnx_path)

with open("proto_files/add_const_init.prototxt", "w") as f:
    f.write(str(model))

print(f"Generated {onnx_path} and add_const_init.prototxt")