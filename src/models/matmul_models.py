import onnx
from onnx import helper, TensorProto
import numpy as np
import os

def generate_matmul_test(name, input_dim, weight_dims):
    """
    name: filename suffix
    input_dim: size of vector x (e.g., 3)
    weight_dims: shape of the second matrix in MatMul (e.g., [3, 2])
    """
    os.makedirs("onnx_files", exist_ok=True)
    
    # 1. INITIALIZER (Match input_dim)
    weight_val = np.random.randn(input_dim).astype(np.float32)
    initializer_tensor = helper.make_tensor(
        name="stem.1.weight",
        data_type=TensorProto.FLOAT,
        dims=[input_dim],
        vals=weight_val.flatten().tolist()
    )

    # 2. CONSTANTS
    # Vector add constant
    val1 = np.ones(input_dim, dtype=np.float32)
    tensor1 = helper.make_tensor(
        name="const_tensor_1",
        data_type=TensorProto.FLOAT,
        dims=[input_dim],
        vals=val1.flatten().tolist()
    )

    # MatMul weight matrix (K x N)
    val2 = np.random.randn(*weight_dims).astype(np.float32)
    tensor2 = helper.make_tensor(
        name="const_tensor_2",
        data_type=TensorProto.FLOAT,
        dims=weight_dims,
        vals=val2.flatten().tolist()
    )

    # 3. NODES
    nodes = [
        helper.make_node("Constant", inputs=[], outputs=["const_out_1"], value=tensor1),
        helper.make_node("Constant", inputs=[], outputs=["const_out_2"], value=tensor2),
        helper.make_node("Add", inputs=["x", "stem.1.weight"], outputs=["out1"]),
        helper.make_node("Add", inputs=["out1", "const_out_1"], outputs=["out2"]),
        helper.make_node("Relu", inputs=["out2"], outputs=["out3"]),
        helper.make_node("MatMul", inputs=["out3", "const_out_2"], outputs=["final_out"])
    ]

    # 4. GRAPH
    # Note: final_out shape will be [weight_dims[1]] because 1xK * KxN = 1xN
    graph = helper.make_graph(
        nodes=nodes,
        name=f"MatMul_{name}",
        inputs=[helper.make_tensor_value_info("x", TensorProto.FLOAT, [input_dim])],
        outputs=[helper.make_tensor_value_info("final_out", TensorProto.FLOAT, [weight_dims[1]])],
        initializer=[initializer_tensor]
    )

    model = helper.make_model(graph, ir_version=8, opset_imports=[helper.make_opsetid("", 18)])
    onnx.save(model, f"onnx_files/{name}.onnx")
    print(f"Generated onnx_files/{name}.onnx")

    with open(f"proto_files/{name}.onnx", "w") as f:
        f.write(str(model))

# --- TEST SUITE ---
# Original-ish: x[2] -> expand[1,2] @ weight[2,1] -> [1]
generate_matmul_test("test_2x1", 2, [2, 1])

# Square matrix: x[2] -> expand[1,2] @ weight[2,2] -> [2]
generate_matmul_test("test_2x2", 2, [2, 2])

# Rectangular: x[3] -> expand[1,3] @ weight[3,2] -> [2]
generate_matmul_test("test_3x2", 3, [3, 2])

# Large Square: x[10] -> expand[1,10] @ weight[10,10] -> [10]
generate_matmul_test("test_10x10", 10, [10, 10])