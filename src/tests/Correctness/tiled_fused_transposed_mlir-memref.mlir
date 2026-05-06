#loc = loc(unknown)
module {
  func.func @main(%arg0: memref<512x512xf32, strided<[?, ?], offset: ?>> loc(unknown), %arg1: memref<512x512xf32, strided<[?, ?], offset: ?>> loc(unknown), %arg2: memref<512x512xf32, strided<[?, ?], offset: ?>> loc(unknown)) -> memref<512x512xf32> attributes {llvm.emit_c_interface} {
    %c1 = arith.constant 1 : index loc(#loc)
    %c64 = arith.constant 64 : index loc(#loc)
    %c512 = arith.constant 512 : index loc(#loc)
    %c0 = arith.constant 0 : index loc(#loc)
    %cst = arith.constant 0.000000e+00 : f32 loc(#loc)
    %alloc = memref.alloc() {alignment = 64 : i64} : memref<512x512xf32> loc(#loc1)
    %alloc_0 = memref.alloc() {alignment = 64 : i64} : memref<512x512xf32> loc(#loc1)
    scf.for %arg3 = %c0 to %c512 step %c1 {
      scf.for %arg4 = %c0 to %c512 step %c1 {
        %0 = memref.load %arg1[%arg4, %arg3] : memref<512x512xf32, strided<[?, ?], offset: ?>> loc(#loc1)
        memref.store %0, %alloc_0[%arg3, %arg4] : memref<512x512xf32> loc(#loc1)
      } loc(#loc1)
    } loc(#loc1)
    scf.for %arg3 = %c0 to %c512 step %c64 {
      scf.for %arg4 = %c0 to %c512 step %c64 {
        %subview = memref.subview %arg0[%arg3, 0] [64, 512] [1, 1] : memref<512x512xf32, strided<[?, ?], offset: ?>> to memref<64x512xf32, strided<[?, ?], offset: ?>> loc(#loc1)
        %subview_1 = memref.subview %alloc_0[%arg4, 0] [64, 512] [1, 1] : memref<512x512xf32> to memref<64x512xf32, strided<[512, 1], offset: ?>> loc(#loc1)
        %alloc_2 = memref.alloc() {alignment = 64 : i64} : memref<64x64xf32> loc(#loc1)
        %alloc_3 = memref.alloc() {alignment = 64 : i64} : memref<64x64xf32> loc(#loc1)
        scf.for %arg5 = %c0 to %c64 step %c1 {
          scf.for %arg6 = %c0 to %c64 step %c1 {
            memref.store %cst, %alloc_3[%arg5, %arg6] : memref<64x64xf32> loc(#loc1)
          } loc(#loc1)
        } loc(#loc1)
        scf.for %arg5 = %c0 to %c512 step %c64 {
          %subview_6 = memref.subview %subview[0, %arg5] [64, 64] [1, 1] : memref<64x512xf32, strided<[?, ?], offset: ?>> to memref<64x64xf32, strided<[?, ?], offset: ?>> loc(#loc1)
          %subview_7 = memref.subview %subview_1[0, %arg5] [64, 64] [1, 1] : memref<64x512xf32, strided<[512, 1], offset: ?>> to memref<64x64xf32, strided<[512, 1], offset: ?>> loc(#loc1)
          scf.for %arg6 = %c0 to %c64 step %c1 {
            scf.for %arg7 = %c0 to %c64 step %c1 {
              scf.for %arg8 = %c0 to %c64 step %c1 {
                %0 = memref.load %subview_6[%arg6, %arg8] : memref<64x64xf32, strided<[?, ?], offset: ?>> loc(#loc1)
                %1 = memref.load %subview_7[%arg7, %arg8] : memref<64x64xf32, strided<[512, 1], offset: ?>> loc(#loc1)
                %2 = memref.load %alloc_3[%arg6, %arg7] : memref<64x64xf32> loc(#loc1)
                %3 = arith.mulf %0, %1 : f32 loc(#loc1)
                %4 = arith.addf %2, %3 : f32 loc(#loc1)
                memref.store %4, %alloc_3[%arg6, %arg7] : memref<64x64xf32> loc(#loc1)
              } loc(#loc1)
            } loc(#loc1)
          } loc(#loc1)
        } loc(#loc1)
        %subview_4 = memref.subview %arg2[%arg3, %arg4] [64, 64] [1, 1] : memref<512x512xf32, strided<[?, ?], offset: ?>> to memref<64x64xf32, strided<[?, ?], offset: ?>> loc(#loc3)
        scf.for %arg5 = %c0 to %c64 step %c1 {
          scf.for %arg6 = %c0 to %c64 step %c1 {
            %0 = memref.load %alloc_3[%arg5, %arg6] : memref<64x64xf32> loc(#loc3)
            %1 = memref.load %subview_4[%arg5, %arg6] : memref<64x64xf32, strided<[?, ?], offset: ?>> loc(#loc3)
            %2 = arith.addf %0, %1 : f32 loc(#loc)
            memref.store %2, %alloc_2[%arg5, %arg6] : memref<64x64xf32> loc(#loc3)
          } loc(#loc3)
        } loc(#loc3)
        %subview_5 = memref.subview %alloc[%arg3, %arg4] [64, 64] [1, 1] : memref<512x512xf32> to memref<64x64xf32, strided<[512, 1], offset: ?>> loc(#loc2)
        scf.for %arg5 = %c0 to %c64 step %c1 {
          scf.for %arg6 = %c0 to %c64 step %c1 {
            %0 = memref.load %alloc_2[%arg5, %arg6] : memref<64x64xf32> loc(#loc2)
            %1 = arith.maxnumf %0, %cst : f32 loc(#loc2)
            memref.store %1, %subview_5[%arg5, %arg6] : memref<64x64xf32, strided<[512, 1], offset: ?>> loc(#loc2)
          } loc(#loc2)
        } loc(#loc2)
        memref.copy %subview_5, %subview_5 : memref<64x64xf32, strided<[512, 1], offset: ?>> to memref<64x64xf32, strided<[512, 1], offset: ?>> loc(#loc2)
      } loc(#loc2)
    } loc(#loc2)
    return %alloc : memref<512x512xf32> loc(#loc2)
  } loc(#loc)
} loc(#loc)
#loc1 = loc("matmul_out")
#loc2 = loc("Output")
#loc3 = loc("add_out")

