#loc = loc(unknown)
module {
  func.func @main(%arg0: tensor<512x512xf32> loc(unknown), %arg1: tensor<512x512xf32> loc(unknown)) -> tensor<512x512xf32> attributes {llvm.emit_c_interface} {
    %cst = arith.constant 0.000000e+00 : f32 loc(#loc1)
    %0 = tensor.empty() : tensor<512x512xf32> loc(#loc1)
    %1 = linalg.fill ins(%cst : f32) outs(%0 : tensor<512x512xf32>) -> tensor<512x512xf32> loc(#loc1)
    %2 = linalg.matmul ins(%arg0, %arg1 : tensor<512x512xf32>, tensor<512x512xf32>) outs(%1 : tensor<512x512xf32>) -> tensor<512x512xf32> loc(#loc1)
    return %2 : tensor<512x512xf32> loc(#loc1)
  } loc(#loc)
} loc(#loc)
#loc1 = loc("Z")

