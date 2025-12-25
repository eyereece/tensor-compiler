import onnx
from onnx import helper, TensorProto
import numpy as np
import os

# Create directory if it doesn't exist
os.makedirs("onnx_files", exist_ok=True)

# 1. Prepare values as NumPy arrays
# first example: "\000\000\000@" is 2.0 in float32
val1 = np.array(2.0, dtype=np.float32)
# second example: "\2757\2065" is a small float (approx 1e-5)
val2 = np.array(1e-5, dtype=np.float32)

# 2. Create Tensors using 'raw_data'
# By passing 'raw_data=...', ONNX will NOT use 'float_data'
tensor1 = helper.make_tensor(
    # name="const_val_1",
    data_type=TensorProto.FLOAT,
    dims=[],  # Empty dims = Scalar (Rank 0)
    vals=val1.tobytes(),
    raw=True  # This flag tells ONNX to treat 'vals' as a byte string
)

tensor2 = helper.make_tensor(
    # name="const_val_2",
    data_type=TensorProto.FLOAT,
    dims=[],
    vals=val2.tobytes(),
    raw=True
)

# 3. Create Constant Nodes
const_node1 = helper.make_node(
    "Constant",
    inputs=[],
    outputs=["out_1"],
    value=tensor1
)

const_node2 = helper.make_node(
    "Constant",
    inputs=[],
    outputs=["out_2"],
    value=tensor2
)

# 4. Create Add Node
add_node = helper.make_node(
    "Add",
    inputs=["out_1", "out_2"],
    outputs=["final_out"]
)

# 5. Create Graph and Model
graph = helper.make_graph(
    nodes=[const_node1, const_node2, add_node],
    name="RawDataConstantGraph",
    inputs=[],
    outputs=[
        helper.make_tensor_value_info("final_out", TensorProto.FLOAT, [])
    ]
)

model = helper.make_model(graph, producer_name="raw-data-test")

# Save
path = "onnx_files/add_constant_1.onnx"
onnx.save(model, path)
print(f"Saved {path}")