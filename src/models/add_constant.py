import onnx
from onnx import helper, TensorProto
import numpy as np

# CONSTANTS
a_value = np.array([1.0, 2.0], dtype=np.float32)
b_value = np.array([3.0, 4.0], dtype=np.float32)

a_tensor = helper.make_tensor(
    name="a",
    data_type=TensorProto.FLOAT,
    dims=a_value.shape,
    vals=a_value.flatten().tolist()
)

b_tensor = helper.make_tensor(
    name="b",
    data_type=TensorProto.FLOAT,
    dims=b_value.shape,
    vals=b_value.flatten().tolist()
)

a_const = helper.make_node(
    "Constant",
    inputs=[],
    outputs=["a_out"],
    value=a_tensor
)

b_const = helper.make_node(
    "Constant",
    inputs=[],
    outputs=["b_out"],
    value=b_tensor
)

# ADD
add_node = helper.make_node(
    "Add",
    inputs=["a_out", "b_out"],
    outputs=["y"]
)

# GRAPH
graph = helper.make_graph(
    nodes=[a_const, b_const, add_node],
    name="AddConstantGraph",
    inputs=[],
    outputs=[
        helper.make_tensor_value_info(
            "y", TensorProto.FLOAT, [2]
        )
    ]
)

model = helper.make_model(graph, producer_name="minimal-add")

onnx.save(model, "onnx_files/add_constant.onnx")
print("Saved add_constant.onnx")