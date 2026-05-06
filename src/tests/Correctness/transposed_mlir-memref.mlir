#loc = loc(unknown)
module {
  func.func @main(%arg0: memref<4096x4096xf32, strided<[?, ?], offset: ?>> loc(unknown), %arg1: memref<4096x4096xf32, strided<[?, ?], offset: ?>> loc(unknown)) -> memref<4096x4096xf32> attributes {llvm.emit_c_interface} {
    %c1 = arith.constant 1 : index loc(#loc)
    %c4096 = arith.constant 4096 : index loc(#loc)
    %c0 = arith.constant 0 : index loc(#loc)
    %cst = arith.constant 0.000000e+00 : f32 loc(#loc1)
    %alloc = memref.alloc() {alignment = 64 : i64} : memref<4096x4096xf32> loc(#loc1)
    %alloc_0 = memref.alloc() {alignment = 64 : i64} : memref<4096x4096xf32> loc(#loc1)
    scf.for %arg2 = %c0 to %c4096 step %c1 {
      scf.for %arg3 = %c0 to %c4096 step %c1 {
        memref.store %cst, %alloc_0[%arg2, %arg3] : memref<4096x4096xf32> loc(#loc1)
      } loc(#loc1)
    } loc(#loc1)
    scf.for %arg2 = %c0 to %c4096 step %c1 {
      scf.for %arg3 = %c0 to %c4096 step %c1 {
        %0 = memref.load %arg1[%arg3, %arg2] : memref<4096x4096xf32, strided<[?, ?], offset: ?>> loc(#loc1)
        memref.store %0, %alloc[%arg2, %arg3] : memref<4096x4096xf32> loc(#loc1)
      } loc(#loc1)
    } loc(#loc1)
    scf.for %arg2 = %c0 to %c4096 step %c1 {
      scf.for %arg3 = %c0 to %c4096 step %c1 {
        scf.for %arg4 = %c0 to %c4096 step %c1 {
          %0 = memref.load %arg0[%arg2, %arg4] : memref<4096x4096xf32, strided<[?, ?], offset: ?>> loc(#loc1)
          %1 = memref.load %alloc[%arg3, %arg4] : memref<4096x4096xf32> loc(#loc1)
          %2 = memref.load %alloc_0[%arg2, %arg3] : memref<4096x4096xf32> loc(#loc1)
          %3 = arith.mulf %0, %1 : f32 loc(#loc1)
          %4 = arith.addf %2, %3 : f32 loc(#loc1)
          memref.store %4, %alloc_0[%arg2, %arg3] : memref<4096x4096xf32> loc(#loc1)
        } loc(#loc1)
      } loc(#loc1)
    } loc(#loc1)
    return %alloc_0 : memref<4096x4096xf32> loc(#loc1)
  } loc(#loc)
} loc(#loc)
#loc1 = loc("Z")

