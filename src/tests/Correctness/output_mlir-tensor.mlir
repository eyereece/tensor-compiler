#loc = loc(unknown)
#loc1 = loc("Z")
#map = affine_map<(d0, d1, d2) -> (d0, d2)>
#map1 = affine_map<(d0, d1, d2) -> (d1, d2)>
#map2 = affine_map<(d0, d1, d2) -> (d0, d1)>
module {
  func.func @main(%arg0: tensor<2048x2048xf32> loc(unknown), %arg1: tensor<2048x2048xf32> loc(unknown)) -> tensor<2048x2048xf32> attributes {llvm.emit_c_interface} {
    %cst = arith.constant 0.000000e+00 : f32 loc(#loc1)
    %0 = tensor.empty() : tensor<2048x2048xf32> loc(#loc1)
    %transposed = linalg.transpose ins(%arg1 : tensor<2048x2048xf32>) outs(%0 : tensor<2048x2048xf32>) permutation = [1, 0]  loc(#loc1)
    %1 = linalg.fill ins(%cst : f32) outs(%0 : tensor<2048x2048xf32>) -> tensor<2048x2048xf32> loc(#loc1)
    %2 = linalg.generic {indexing_maps = [#map, #map1, #map2], iterator_types = ["parallel", "parallel", "reduction"]} ins(%arg0, %transposed : tensor<2048x2048xf32>, tensor<2048x2048xf32>) outs(%1 : tensor<2048x2048xf32>) {
    ^bb0(%in: f32 loc(unknown), %in_0: f32 loc("Z"), %out: f32 loc("Z")):
      %3 = arith.mulf %in, %in_0 : f32 loc(#loc1)
      %4 = arith.addf %out, %3 : f32 loc(#loc1)
      linalg.yield %4 : f32 loc(#loc1)
    } -> tensor<2048x2048xf32> loc(#loc1)
    return %2 : tensor<2048x2048xf32> loc(#loc1)
  } loc(#loc)
} loc(#loc)

