#loc = loc(unknown)
module {
  func.func @main(%arg0: tensor<2048x2048xf32> loc(unknown), %arg1: tensor<2048x2048xf32> loc(unknown)) -> tensor<2048x2048xf32> attributes {llvm.emit_c_interface} {
    %0 = "dlc.matmul"(%arg0, %arg1) : (tensor<2048x2048xf32>, tensor<2048x2048xf32>) -> tensor<2048x2048xf32> loc(#loc1)
    return %0 : tensor<2048x2048xf32> loc(#loc1)
  } loc(#loc)
} loc(#loc)
#loc1 = loc("Z")

