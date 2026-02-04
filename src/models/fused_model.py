import onnx
from onnx import helper, TensorProto
import numpy as np

def create_fused_matmul(dim):
    # Define Value infos (Inputs and Outputs)
    X = helper.make_tensor_value_info('X', TensorProto.FLOAT, [dim, dim])
    Y = helper.make_tensor_value_info('Y', TensorProto.FLOAT, [dim, dim])
    Bias = helper.make_tensor_value_info('Bias', TensorProto.FLOAT, [dim, dim])
    Final_Output = helper.make_tensor_value_info('Output', TensorProto.FLOAT, [dim, dim])

    # Node 1: MatMul (X * Y -> matmul_out)
    matmul_node = helper.make_node(
        'MatMul',
        ['X', 'Y'],
        ['matmul_out']
    )

    # Node 2: Add (matmul_out + bias -> add_out)
    add_node = helper.make_node(
        'Add',
        ['matmul_out', 'Bias'],
        ['add_out']
    )

    # Node 3: ReLu (add_out -> output)
    relu_node = helper.make_node(
        'Relu',
        ['add_out'],
        ['Output']
    )

    graph = helper.make_graph(
        [matmul_node, add_node, relu_node],
        'fused_matmul_graph',
        [X, Y, Bias],
        [Final_Output]
    )

    # Save model
    opset_version = helper.make_opsetid("", 11)
    model = helper.make_model(graph, ir_version=8, opset_imports=[opset_version])

    filename = f'onnx_files/fused_matmul_{dim}.onnx'
    onnx.save(model, filename)
    print(f"Created {filename} for fusion testing.")

create_fused_matmul(4096)