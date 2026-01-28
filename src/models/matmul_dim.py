import onnx
from onnx import helper, TensorProto

def create_large_matmul(dim=2048):
    # inputs: A[dim, dim], B[dim, dim]
    X = helper.make_tensor_value_info('X', TensorProto.FLOAT, [dim, dim])
    Y = helper.make_tensor_value_info('Y', TensorProto.FLOAT, [dim, dim])
    Z = helper.make_tensor_value_info('Z', TensorProto.FLOAT, [dim, dim])

    node = helper.make_node('MatMul', ['X', 'Y'], ['Z'])
    graph = helper.make_graph([node], 'large_matmul_graph', [X, Y], [Z])
    opset_version = helper.make_opsetid("", 11)
    model = helper.make_model(graph, producer_name='bench_gen', ir_version=8, opset_imports=[opset_version])

    onnx.save(model, f'onnx_files/matmul_{dim}.onnx')
    print(f"Created matmul_{dim}.onnx")

create_large_matmul(2048)