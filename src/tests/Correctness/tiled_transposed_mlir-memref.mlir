#loc = loc(unknown)
module {
  func.func @main(%arg0: memref<2048x2048xf32, strided<[?, ?], offset: ?>> loc(unknown), %arg1: memref<2048x2048xf32, strided<[?, ?], offset: ?>> loc(unknown), %arg2: memref<2048x2048xf32, strided<[?, ?], offset: ?>> loc(unknown)) -> memref<2048x2048xf32> attributes {llvm.emit_c_interface} {
    %c1 = arith.constant 1 : index loc(#loc)
    %c64 = arith.constant 64 : index loc(#loc)
    %c2048 = arith.constant 2048 : index loc(#loc)
    %c0 = arith.constant 0 : index loc(#loc)
    %cst = arith.constant 0.000000e+00 : f32 loc(#loc)
    %alloc = memref.alloc() {alignment = 64 : i64} : memref<2048x2048xf32> loc(#loc1)
    %alloc_0 = memref.alloc() {alignment = 64 : i64} : memref<2048x2048xf32> loc(#loc1)
    scf.for %arg3 = %c0 to %c2048 step %c1 {
      scf.for %arg4 = %c0 to %c2048 step %c1 {
        memref.store %cst, %alloc_0[%arg3, %arg4] : memref<2048x2048xf32> loc(#loc1)
      } loc(#loc1)
    } loc(#loc1)
    scf.for %arg3 = %c0 to %c2048 step %c1 {
      scf.for %arg4 = %c0 to %c2048 step %c1 {
        %0 = memref.load %arg1[%arg4, %arg3] : memref<2048x2048xf32, strided<[?, ?], offset: ?>> loc(#loc1)
        memref.store %0, %alloc[%arg3, %arg4] : memref<2048x2048xf32> loc(#loc1)
      } loc(#loc1)
    } loc(#loc1)
    scf.for %arg3 = %c0 to %c2048 step %c64 {
      scf.for %arg4 = %c0 to %c2048 step %c64 {
        scf.for %arg5 = %c0 to %c2048 step %c64 {
          %subview = memref.subview %arg0[%arg3, %arg5] [64, 64] [1, 1] : memref<2048x2048xf32, strided<[?, ?], offset: ?>> to memref<64x64xf32, strided<[?, ?], offset: ?>> loc(#loc1)
          %subview_1 = memref.subview %alloc[%arg4, %arg5] [64, 64] [1, 1] : memref<2048x2048xf32> to memref<64x64xf32, strided<[2048, 1], offset: ?>> loc(#loc1)
          %subview_2 = memref.subview %alloc_0[%arg3, %arg4] [64, 64] [1, 1] : memref<2048x2048xf32> to memref<64x64xf32, strided<[2048, 1], offset: ?>> loc(#loc1)
          scf.for %arg6 = %c0 to %c64 step %c1 {
            scf.for %arg7 = %c0 to %c64 step %c1 {
              scf.for %arg8 = %c0 to %c64 step %c1 {
                %0 = memref.load %subview[%arg6, %arg8] : memref<64x64xf32, strided<[?, ?], offset: ?>> loc(#loc1)
                %1 = memref.load %subview_1[%arg7, %arg8] : memref<64x64xf32, strided<[2048, 1], offset: ?>> loc(#loc1)
                %2 = memref.load %subview_2[%arg6, %arg7] : memref<64x64xf32, strided<[2048, 1], offset: ?>> loc(#loc1)
                %3 = arith.mulf %0, %1 : f32 loc(#loc1)
                %4 = arith.addf %2, %3 : f32 loc(#loc1)
                memref.store %4, %subview_2[%arg6, %arg7] : memref<64x64xf32, strided<[2048, 1], offset: ?>> loc(#loc1)
              } loc(#loc1)
            } loc(#loc1)
          } loc(#loc1)
        } loc(#loc1)
      } loc(#loc1)
    } loc(#loc1)
    scf.for %arg3 = %c0 to %c2048 step %c1 {
      scf.for %arg4 = %c0 to %c2048 step %c1 {
        %0 = memref.load %alloc_0[%arg3, %arg4] : memref<2048x2048xf32> loc(#loc2)
        %1 = memref.load %arg2[%arg3, %arg4] : memref<2048x2048xf32, strided<[?, ?], offset: ?>> loc(#loc2)
        %2 = arith.addf %0, %1 : f32 loc(#loc)
        memref.store %2, %alloc[%arg3, %arg4] : memref<2048x2048xf32> loc(#loc2)
      } loc(#loc2)
    } loc(#loc2)
    scf.for %arg3 = %c0 to %c2048 step %c1 {
      scf.for %arg4 = %c0 to %c2048 step %c1 {
        %0 = memref.load %alloc[%arg3, %arg4] : memref<2048x2048xf32> loc(#loc3)
        %1 = arith.maxnumf %0, %cst : f32 loc(#loc3)
        memref.store %1, %alloc[%arg3, %arg4] : memref<2048x2048xf32> loc(#loc3)
      } loc(#loc3)
    } loc(#loc3)
    return %alloc : memref<2048x2048xf32> loc(#loc3)
  } loc(#loc)
} loc(#loc)
#loc1 = loc("matmul_out")
#loc2 = loc("add_out")
#loc3 = loc("Output")

