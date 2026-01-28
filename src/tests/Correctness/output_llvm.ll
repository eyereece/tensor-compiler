; ModuleID = 'LLVMDialectModule'
source_filename = "LLVMDialectModule"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i8:8:32-i16:16:32-i64:64-i128:128-n32:64-S128-Fn32"
target triple = "aarch64-unknown-linux-gnu"

declare ptr @malloc(i64)

define { ptr, ptr, i64, [2 x i64], [2 x i64] } @main(ptr %0, ptr %1, i64 %2, i64 %3, i64 %4, i64 %5, i64 %6, ptr %7, ptr %8, i64 %9, i64 %10, i64 %11, i64 %12, i64 %13) {
  %15 = insertvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } poison, ptr %7, 0
  %16 = insertvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %15, ptr %8, 1
  %17 = insertvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %16, i64 %9, 2
  %18 = insertvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %17, i64 %10, 3, 0
  %19 = insertvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %18, i64 %12, 4, 0
  %20 = insertvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %19, i64 %11, 3, 1
  %21 = insertvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %20, i64 %13, 4, 1
  %22 = insertvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } poison, ptr %0, 0
  %23 = insertvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %22, ptr %1, 1
  %24 = insertvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %23, i64 %2, 2
  %25 = insertvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %24, i64 %3, 3, 0
  %26 = insertvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %25, i64 %5, 4, 0
  %27 = insertvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %26, i64 %4, 3, 1
  %28 = insertvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %27, i64 %6, 4, 1
  %29 = call ptr @malloc(i64 1048640)
  %30 = ptrtoint ptr %29 to i64
  %31 = add i64 %30, 63
  %32 = urem i64 %31, 64
  %33 = sub i64 %31, %32
  %34 = inttoptr i64 %33 to ptr
  %35 = insertvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } poison, ptr %29, 0
  %36 = insertvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %35, ptr %34, 1
  %37 = insertvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %36, i64 0, 2
  %38 = insertvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %37, i64 512, 3, 0
  %39 = insertvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %38, i64 512, 3, 1
  %40 = insertvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %39, i64 512, 4, 0
  %41 = insertvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %40, i64 1, 4, 1
  br label %42

42:                                               ; preds = %55, %14
  %43 = phi i64 [ 0, %14 ], [ %56, %55 ]
  %44 = icmp slt i64 %43, 512
  br i1 %44, label %45, label %57

45:                                               ; preds = %42
  br label %46

46:                                               ; preds = %49, %45
  %47 = phi i64 [ 0, %45 ], [ %54, %49 ]
  %48 = icmp slt i64 %47, 512
  br i1 %48, label %49, label %55

49:                                               ; preds = %46
  %50 = extractvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %41, 1
  %51 = mul nuw nsw i64 %43, 512
  %52 = add nuw nsw i64 %51, %47
  %53 = getelementptr inbounds nuw float, ptr %50, i64 %52
  store float 0.000000e+00, ptr %53, align 4
  %54 = add i64 %47, 1
  br label %46

55:                                               ; preds = %46
  %56 = add i64 %43, 1
  br label %42

57:                                               ; preds = %42
  br label %58

58:                                               ; preds = %104, %57
  %59 = phi i64 [ 0, %57 ], [ %105, %104 ]
  %60 = icmp slt i64 %59, 512
  br i1 %60, label %61, label %106

61:                                               ; preds = %58
  br label %62

62:                                               ; preds = %102, %61
  %63 = phi i64 [ 0, %61 ], [ %103, %102 ]
  %64 = icmp slt i64 %63, 512
  br i1 %64, label %65, label %104

65:                                               ; preds = %62
  br label %66

66:                                               ; preds = %69, %65
  %67 = phi i64 [ 0, %65 ], [ %101, %69 ]
  %68 = icmp slt i64 %67, 512
  br i1 %68, label %69, label %102

69:                                               ; preds = %66
  %70 = extractvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %28, 1
  %71 = extractvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %28, 2
  %72 = getelementptr float, ptr %70, i64 %71
  %73 = extractvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %28, 4, 0
  %74 = mul nuw nsw i64 %59, %73
  %75 = extractvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %28, 4, 1
  %76 = mul nuw nsw i64 %67, %75
  %77 = add nuw nsw i64 %74, %76
  %78 = getelementptr inbounds nuw float, ptr %72, i64 %77
  %79 = load float, ptr %78, align 4
  %80 = extractvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %21, 1
  %81 = extractvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %21, 2
  %82 = getelementptr float, ptr %80, i64 %81
  %83 = extractvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %21, 4, 0
  %84 = mul nuw nsw i64 %67, %83
  %85 = extractvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %21, 4, 1
  %86 = mul nuw nsw i64 %63, %85
  %87 = add nuw nsw i64 %84, %86
  %88 = getelementptr inbounds nuw float, ptr %82, i64 %87
  %89 = load float, ptr %88, align 4
  %90 = extractvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %41, 1
  %91 = mul nuw nsw i64 %59, 512
  %92 = add nuw nsw i64 %91, %63
  %93 = getelementptr inbounds nuw float, ptr %90, i64 %92
  %94 = load float, ptr %93, align 4
  %95 = fmul float %79, %89
  %96 = fadd float %94, %95
  %97 = extractvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %41, 1
  %98 = mul nuw nsw i64 %59, 512
  %99 = add nuw nsw i64 %98, %63
  %100 = getelementptr inbounds nuw float, ptr %97, i64 %99
  store float %96, ptr %100, align 4
  %101 = add i64 %67, 1
  br label %66

102:                                              ; preds = %66
  %103 = add i64 %63, 1
  br label %62

104:                                              ; preds = %62
  %105 = add i64 %59, 1
  br label %58

106:                                              ; preds = %58
  ret { ptr, ptr, i64, [2 x i64], [2 x i64] } %41
}

define void @_mlir_ciface_main(ptr %0, ptr %1, ptr %2) {
  %4 = load { ptr, ptr, i64, [2 x i64], [2 x i64] }, ptr %1, align 8
  %5 = extractvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %4, 0
  %6 = extractvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %4, 1
  %7 = extractvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %4, 2
  %8 = extractvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %4, 3, 0
  %9 = extractvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %4, 3, 1
  %10 = extractvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %4, 4, 0
  %11 = extractvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %4, 4, 1
  %12 = load { ptr, ptr, i64, [2 x i64], [2 x i64] }, ptr %2, align 8
  %13 = extractvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %12, 0
  %14 = extractvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %12, 1
  %15 = extractvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %12, 2
  %16 = extractvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %12, 3, 0
  %17 = extractvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %12, 3, 1
  %18 = extractvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %12, 4, 0
  %19 = extractvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %12, 4, 1
  %20 = call { ptr, ptr, i64, [2 x i64], [2 x i64] } @main(ptr %5, ptr %6, i64 %7, i64 %8, i64 %9, i64 %10, i64 %11, ptr %13, ptr %14, i64 %15, i64 %16, i64 %17, i64 %18, i64 %19)
  store { ptr, ptr, i64, [2 x i64], [2 x i64] } %20, ptr %0, align 8
  ret void
}

!llvm.module.flags = !{!0}

!0 = !{i32 2, !"Debug Info Version", i32 3}

