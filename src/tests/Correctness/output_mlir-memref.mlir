#loc = loc(unknown)
module {
  func.func @main(%arg0: memref<512x512xf32, strided<[?, ?], offset: ?>> loc(unknown), %arg1: memref<512x512xf32, strided<[?, ?], offset: ?>> loc(unknown)) -> memref<512x512xf32> attributes {llvm.emit_c_interface} {
    %c1 = arith.constant 1 : index loc(#loc)
    %c512 = arith.constant 512 : index loc(#loc)
    %c0 = arith.constant 0 : index loc(#loc)
    %cst = arith.constant 0.000000e+00 : f32 loc(#loc1)
    %alloc = memref.alloc() {alignment = 64 : i64} : memref<512x512xf32> loc(#loc1)
    scf.for %arg2 = %c0 to %c512 step %c1 {
      scf.for %arg3 = %c0 to %c512 step %c1 {
        memref.store %cst, %alloc[%arg2, %arg3] : memref<512x512xf32> loc(#loc1)
      } loc(#loc1)
    } loc(#loc1)
    scf.for %arg2 = %c0 to %c512 step %c1 {
      scf.for %arg3 = %c0 to %c512 step %c1 {
        scf.for %arg4 = %c0 to %c512 step %c1 {
          %0 = memref.load %arg0[%arg2, %arg4] : memref<512x512xf32, strided<[?, ?], offset: ?>> loc(#loc1)
          %1 = memref.load %arg1[%arg4, %arg3] : memref<512x512xf32, strided<[?, ?], offset: ?>> loc(#loc1)
          %2 = memref.load %alloc[%arg2, %arg3] : memref<512x512xf32> loc(#loc1)
          %3 = arith.mulf %0, %1 : f32 loc(#loc)
          %4 = arith.addf %2, %3 : f32 loc(#loc)
          memref.store %4, %alloc[%arg2, %arg3] : memref<512x512xf32> loc(#loc1)
        } loc(#loc1)
      } loc(#loc1)
    } loc(#loc1)
    return %alloc : memref<512x512xf32> loc(#loc1)
  } loc(#loc)
} loc(#loc)
#loc1 = loc("Z")

