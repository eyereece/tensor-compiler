#loc = loc(unknown)
module {
  func.func @main(%arg0: tensor<512x512xf32> loc(unknown), %arg1: tensor<512x512xf32> loc(unknown), %arg2: tensor<512x512xf32> loc(unknown)) -> tensor<512x512xf32> attributes {llvm.emit_c_interface} {
    %0 = "dlc.matmul"(%arg0, %arg1) : (tensor<512x512xf32>, tensor<512x512xf32>) -> tensor<512x512xf32> loc(#loc1)
    %1 = "dlc.add"(%0, %arg2) : (tensor<512x512xf32>, tensor<512x512xf32>) -> tensor<512x512xf32> loc(#loc2)
    %2 = dlc.relu %1 : tensor<512x512xf32> loc(#loc3)
    return %2 : tensor<512x512xf32> loc(#loc3)
  } loc(#loc)
} loc(#loc)
#loc1 = loc("matmul_out")
#loc2 = loc("add_out")
#loc3 = loc("Output")

