#loc = loc(unknown)
module {
  func.func @main(%arg0: memref<512x512xf32, strided<[?, ?], offset: ?>> loc(unknown), %arg1: memref<512x512xf32, strided<[?, ?], offset: ?>> loc(unknown), %arg2: memref<512x512xf32, strided<[?, ?], offset: ?>> loc(unknown)) -> memref<512x512xf32> attributes {llvm.emit_c_interface} {
    %c1 = arith.constant 1 : index loc(#loc)
    %c512 = arith.constant 512 : index loc(#loc)
    %c0 = arith.constant 0 : index loc(#loc)
    %cst = arith.constant 0.000000e+00 : f32 loc(#loc)
    %alloc = memref.alloc() {alignment = 64 : i64} : memref<512x512xf32> loc(#loc1)
    scf.for %arg3 = %c0 to %c512 step %c1 {
      scf.for %arg4 = %c0 to %c512 step %c1 {
        memref.store %cst, %alloc[%arg3, %arg4] : memref<512x512xf32> loc(#loc1)
      } loc(#loc1)
    } loc(#loc1)
    scf.for %arg3 = %c0 to %c512 step %c1 {
      scf.for %arg4 = %c0 to %c512 step %c1 {
        scf.for %arg5 = %c0 to %c512 step %c1 {
          %0 = memref.load %arg0[%arg3, %arg5] : memref<512x512xf32, strided<[?, ?], offset: ?>> loc(#loc1)
          %1 = memref.load %arg1[%arg5, %arg4] : memref<512x512xf32, strided<[?, ?], offset: ?>> loc(#loc1)
          %2 = memref.load %alloc[%arg3, %arg4] : memref<512x512xf32> loc(#loc1)
          %3 = arith.mulf %0, %1 : f32 loc(#loc)
          %4 = arith.addf %2, %3 : f32 loc(#loc)
          memref.store %4, %alloc[%arg3, %arg4] : memref<512x512xf32> loc(#loc1)
        } loc(#loc1)
      } loc(#loc1)
    } loc(#loc1)
    scf.for %arg3 = %c0 to %c512 step %c1 {
      scf.for %arg4 = %c0 to %c512 step %c1 {
        %0 = memref.load %alloc[%arg3, %arg4] : memref<512x512xf32> loc(#loc2)
        %1 = memref.load %arg2[%arg3, %arg4] : memref<512x512xf32, strided<[?, ?], offset: ?>> loc(#loc2)
        %2 = arith.addf %0, %1 : f32 loc(#loc)
        memref.store %2, %alloc[%arg3, %arg4] : memref<512x512xf32> loc(#loc2)
      } loc(#loc2)
    } loc(#loc2)
    scf.for %arg3 = %c0 to %c512 step %c1 {
      scf.for %arg4 = %c0 to %c512 step %c1 {
        %0 = memref.load %alloc[%arg3, %arg4] : memref<512x512xf32> loc(#loc3)
        %1 = arith.maxnumf %0, %cst : f32 loc(#loc3)
        memref.store %1, %alloc[%arg3, %arg4] : memref<512x512xf32> loc(#loc3)
      } loc(#loc3)
    } loc(#loc3)
    return %alloc : memref<512x512xf32> loc(#loc3)
  } loc(#loc)
} loc(#loc)
#loc1 = loc("matmul_out")
#loc2 = loc("add_out")
#loc3 = loc("Output")

