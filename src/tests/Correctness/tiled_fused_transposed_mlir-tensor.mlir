#loc = loc(unknown)
#loc1 = loc("matmul_out")
#loc2 = loc("Output")
#loc3 = loc("add_out")
#map = affine_map<(d0, d1, d2) -> (d0, d2)>
#map1 = affine_map<(d0, d1, d2) -> (d1, d2)>
#map2 = affine_map<(d0, d1, d2) -> (d0, d1)>
#map3 = affine_map<(d0, d1) -> (d0, d1)>
module {
  func.func @main(%arg0: tensor<512x512xf32> loc(unknown), %arg1: tensor<512x512xf32> loc(unknown), %arg2: tensor<512x512xf32> loc(unknown)) -> tensor<512x512xf32> attributes {llvm.emit_c_interface} {
    %c64 = arith.constant 64 : index loc(#loc)
    %c512 = arith.constant 512 : index loc(#loc)
    %c0 = arith.constant 0 : index loc(#loc)
    %cst = arith.constant 0.000000e+00 : f32 loc(#loc)
    %0 = tensor.empty() : tensor<512x512xf32> loc(#loc1)
    %transposed = linalg.transpose ins(%arg1 : tensor<512x512xf32>) outs(%0 : tensor<512x512xf32>) permutation = [1, 0]  loc(#loc1)
    %1 = scf.for %arg3 = %c0 to %c512 step %c64 iter_args(%arg4 = %0) -> (tensor<512x512xf32>) {
      %2 = scf.for %arg5 = %c0 to %c512 step %c64 iter_args(%arg6 = %arg4) -> (tensor<512x512xf32>) {
        %extracted_slice = tensor.extract_slice %arg0[%arg3, 0] [64, 512] [1, 1] : tensor<512x512xf32> to tensor<64x512xf32> loc(#loc1)
        %extracted_slice_0 = tensor.extract_slice %transposed[%arg5, 0] [64, 512] [1, 1] : tensor<512x512xf32> to tensor<64x512xf32> loc(#loc1)
        %extracted_slice_1 = tensor.extract_slice %0[%arg3, %arg5] [64, 64] [1, 1] : tensor<512x512xf32> to tensor<64x64xf32> loc(#loc1)
        %3 = linalg.fill ins(%cst : f32) outs(%extracted_slice_1 : tensor<64x64xf32>) -> tensor<64x64xf32> loc(#loc1)
        %4 = scf.for %arg7 = %c0 to %c512 step %c64 iter_args(%arg8 = %3) -> (tensor<64x64xf32>) {
          %extracted_slice_4 = tensor.extract_slice %extracted_slice[0, %arg7] [64, 64] [1, 1] : tensor<64x512xf32> to tensor<64x64xf32> loc(#loc1)
          %extracted_slice_5 = tensor.extract_slice %extracted_slice_0[0, %arg7] [64, 64] [1, 1] : tensor<64x512xf32> to tensor<64x64xf32> loc(#loc1)
          %7 = linalg.generic {indexing_maps = [#map, #map1, #map2], iterator_types = ["parallel", "parallel", "reduction"]} ins(%extracted_slice_4, %extracted_slice_5 : tensor<64x64xf32>, tensor<64x64xf32>) outs(%arg8 : tensor<64x64xf32>) {
          ^bb0(%in: f32 loc(unknown), %in_6: f32 loc("matmul_out"), %out: f32 loc("matmul_out")):
            %8 = arith.mulf %in, %in_6 : f32 loc(#loc1)
            %9 = arith.addf %out, %8 : f32 loc(#loc1)
            linalg.yield %9 : f32 loc(#loc1)
          } -> tensor<64x64xf32> loc(#loc1)
          scf.yield %7 : tensor<64x64xf32> loc(#loc1)
        } loc(#loc1)
        %extracted_slice_2 = tensor.extract_slice %arg2[%arg3, %arg5] [64, 64] [1, 1] : tensor<512x512xf32> to tensor<64x64xf32> loc(#loc3)
        %5 = linalg.add ins(%4, %extracted_slice_2 : tensor<64x64xf32>, tensor<64x64xf32>) outs(%extracted_slice_1 : tensor<64x64xf32>) -> tensor<64x64xf32> loc(#loc3)
        %extracted_slice_3 = tensor.extract_slice %arg6[%arg3, %arg5] [64, 64] [1, 1] : tensor<512x512xf32> to tensor<64x64xf32> loc(#loc2)
        %6 = linalg.generic {indexing_maps = [#map3, #map3], iterator_types = ["parallel", "parallel"]} ins(%5 : tensor<64x64xf32>) outs(%extracted_slice_3 : tensor<64x64xf32>) {
        ^bb0(%in: f32 loc("add_out"), %out: f32 loc("Output")):
          %7 = arith.maxnumf %in, %cst : f32 loc(#loc2)
          linalg.yield %7 : f32 loc(#loc2)
        } -> tensor<64x64xf32> loc(#loc2)
        %inserted_slice = tensor.insert_slice %6 into %arg6[%arg3, %arg5] [64, 64] [1, 1] : tensor<64x64xf32> into tensor<512x512xf32> loc(#loc2)
        scf.yield %inserted_slice : tensor<512x512xf32> loc(#loc2)
      } loc(#loc2)
      scf.yield %2 : tensor<512x512xf32> loc(#loc2)
    } loc(#loc2)
    return %1 : tensor<512x512xf32> loc(#loc2)
  } loc(#loc)
} loc(#loc)

