#loc = loc(unknown)
#loc2 = loc("add_out")
#loc3 = loc("Output")
#map = affine_map<(d0, d1) -> (d0, d1)>
module {
  func.func @main(%arg0: tensor<512x512xf32> loc(unknown), %arg1: tensor<512x512xf32> loc(unknown), %arg2: tensor<512x512xf32> loc(unknown)) -> tensor<512x512xf32> attributes {llvm.emit_c_interface} {
    %cst = arith.constant 0.000000e+00 : f32 loc(#loc)
    %0 = tensor.empty() : tensor<512x512xf32> loc(#loc1)
    %1 = linalg.fill ins(%cst : f32) outs(%0 : tensor<512x512xf32>) -> tensor<512x512xf32> loc(#loc1)
    %2 = linalg.matmul ins(%arg0, %arg1 : tensor<512x512xf32>, tensor<512x512xf32>) outs(%1 : tensor<512x512xf32>) -> tensor<512x512xf32> loc(#loc1)
    %3 = linalg.add ins(%2, %arg2 : tensor<512x512xf32>, tensor<512x512xf32>) outs(%0 : tensor<512x512xf32>) -> tensor<512x512xf32> loc(#loc2)
    %4 = linalg.generic {indexing_maps = [#map, #map], iterator_types = ["parallel", "parallel"]} ins(%3 : tensor<512x512xf32>) outs(%0 : tensor<512x512xf32>) {
    ^bb0(%in: f32 loc("add_out"), %out: f32 loc("Output")):
      %5 = arith.maxnumf %in, %cst : f32 loc(#loc3)
      linalg.yield %5 : f32 loc(#loc3)
    } -> tensor<512x512xf32> loc(#loc3)
    return %4 : tensor<512x512xf32> loc(#loc3)
  } loc(#loc)
} loc(#loc)
#loc1 = loc("matmul_out")

