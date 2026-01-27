import onnx
from onnx import helper, TensorProto
import numpy as np

# MATMUL: [3,2] @ [2, 3] = [3, 3]

# 1. INITIALIZER: Weights [3, 2]
# M=3, K=2
weight_val = np.array([[1, 2], [3, 4], [5, 6]], dtype=np.float32)
initializer_tensor = helper.make_tensor(
    name="weights",
    data_type=TensorProto.FLOAT,
    dims=[3, 2],
    vals=weight_val.tobytes(),
    raw=True
)

# 2. CONSTANT for MatMul: [2, 3]
# K=2, N=3
mat2_val = np.array([[1, 0, 1], [0, 1, 0]], dtype=np.float32)
tensor2 = helper.make_tensor(
    name="const_matrix_2",
    data_type=TensorProto.FLOAT,
    dims=[2, 3],
    vals=mat2_val.tobytes(),
    raw=True
)

const_node_2 = helper.make_node("Constant", inputs=[], outputs=["mat2_out"], value=tensor2)

# 3. NODES
# x is [3, 2]. x + weights = [3, 2]
add_node = helper.make_node("Add", inputs=["x", "weights"], outputs=["sum_out"])

# [3, 2] @ [2, 3] -> [3, 3]
matmul_node = helper.make_node("MatMul", inputs=["sum_out", "mat2_out"], outputs=["final_out"])

# 4. GRAPH
graph = helper.make_graph(
    nodes=[const_node_2, add_node, matmul_node],
    name="RectangularMatMul",
    inputs=[helper.make_tensor_value_info("x", TensorProto.FLOAT, [3, 2])],
    outputs=[helper.make_tensor_value_info("final_out", TensorProto.FLOAT, [3, 3])],
    initializer=[initializer_tensor]
)

model = helper.make_model(graph, ir_version=8, opset_imports=[helper.make_opsetid("", 11)])
onnx.save(model, "onnx_files/matmul_3x3.onnx")
print("Generated onnx_files/matmul_3x3.onnx (Result shape: [3, 3])")