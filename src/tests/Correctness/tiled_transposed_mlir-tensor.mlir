#loc = loc(unknown)
#loc1 = loc("matmul_out")
#loc2 = loc("add_out")
#loc3 = loc("Output")
#map = affine_map<(d0, d1, d2) -> (d0, d2)>
#map1 = affine_map<(d0, d1, d2) -> (d1, d2)>
#map2 = affine_map<(d0, d1, d2) -> (d0, d1)>
#map3 = affine_map<(d0, d1) -> (d0, d1)>
module {
  func.func @main(%arg0: tensor<2048x2048xf32> loc(unknown), %arg1: tensor<2048x2048xf32> loc(unknown), %arg2: tensor<2048x2048xf32> loc(unknown)) -> tensor<2048x2048xf32> attributes {llvm.emit_c_interface} {
    %c64 = arith.constant 64 : index loc(#loc)
    %c2048 = arith.constant 2048 : index loc(#loc)
    %c0 = arith.constant 0 : index loc(#loc)
    %cst = arith.constant 0.000000e+00 : f32 loc(#loc)
    %0 = tensor.empty() : tensor<2048x2048xf32> loc(#loc1)
    %1 = linalg.fill ins(%cst : f32) outs(%0 : tensor<2048x2048xf32>) -> tensor<2048x2048xf32> loc(#loc1)
    %transposed = linalg.transpose ins(%arg1 : tensor<2048x2048xf32>) outs(%0 : tensor<2048x2048xf32>) permutation = [1, 0]  loc(#loc1)
    %2 = scf.for %arg3 = %c0 to %c2048 step %c64 iter_args(%arg4 = %1) -> (tensor<2048x2048xf32>) {
      %5 = scf.for %arg5 = %c0 to %c2048 step %c64 iter_args(%arg6 = %arg4) -> (tensor<2048x2048xf32>) {
        %6 = scf.for %arg7 = %c0 to %c2048 step %c64 iter_args(%arg8 = %arg6) -> (tensor<2048x2048xf32>) {
          %extracted_slice = tensor.extract_slice %arg0[%arg3, %arg7] [64, 64] [1, 1] : tensor<2048x2048xf32> to tensor<64x64xf32> loc(#loc1)
          %extracted_slice_0 = tensor.extract_slice %transposed[%arg5, %arg7] [64, 64] [1, 1] : tensor<2048x2048xf32> to tensor<64x64xf32> loc(#loc1)
          %extracted_slice_1 = tensor.extract_slice %arg8[%arg3, %arg5] [64, 64] [1, 1] : tensor<2048x2048xf32> to tensor<64x64xf32> loc(#loc1)
          %7 = linalg.generic {indexing_maps = [#map, #map1, #map2], iterator_types = ["parallel", "parallel", "reduction"]} ins(%extracted_slice, %extracted_slice_0 : tensor<64x64xf32>, tensor<64x64xf32>) outs(%extracted_slice_1 : tensor<64x64xf32>) {
          ^bb0(%in: f32 loc(unknown), %in_2: f32 loc("matmul_out"), %out: f32 loc("matmul_out")):
            %8 = arith.mulf %in, %in_2 : f32 loc(#loc1)
            %9 = arith.addf %out, %8 : f32 loc(#loc1)
            linalg.yield %9 : f32 loc(#loc1)
          } -> tensor<64x64xf32> loc(#loc1)
          %inserted_slice = tensor.insert_slice %7 into %arg8[%arg3, %arg5] [64, 64] [1, 1] : tensor<64x64xf32> into tensor<2048x2048xf32> loc(#loc1)
          scf.yield %inserted_slice : tensor<2048x2048xf32> loc(#loc1)
        } loc(#loc1)
        scf.yield %6 : tensor<2048x2048xf32> loc(#loc1)
      } loc(#loc1)
      scf.yield %5 : tensor<2048x2048xf32> loc(#loc1)
    } loc(#loc1)
    %3 = linalg.add ins(%2, %arg2 : tensor<2048x2048xf32>, tensor<2048x2048xf32>) outs(%0 : tensor<2048x2048xf32>) -> tensor<2048x2048xf32> loc(#loc2)
    %4 = linalg.generic {indexing_maps = [#map3, #map3], iterator_types = ["parallel", "parallel"]} ins(%3 : tensor<2048x2048xf32>) outs(%0 : tensor<2048x2048xf32>) {
    ^bb0(%in: f32 loc("add_out"), %out: f32 loc("Output")):
      %5 = arith.maxnumf %in, %cst : f32 loc(#loc3)
      linalg.yield %5 : f32 loc(#loc3)
    } -> tensor<2048x2048xf32> loc(#loc3)
    return %4 : tensor<2048x2048xf32> loc(#loc3)
  } loc(#loc)
} loc(#loc)

