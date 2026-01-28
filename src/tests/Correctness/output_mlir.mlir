#loc = loc(unknown)
module {
  func.func @main(%arg0: tensor<512x512xf32> loc(unknown), %arg1: tensor<512x512xf32> loc(unknown)) -> tensor<512x512xf32> attributes {llvm.emit_c_interface} {
    %0 = "dlc.matmul"(%arg0, %arg1) : (tensor<512x512xf32>, tensor<512x512xf32>) -> tensor<512x512xf32> loc(#loc1)
    return %0 : tensor<512x512xf32> loc(#loc1)
  } loc(#loc)
} loc(#loc)
#loc1 = loc("Z")

