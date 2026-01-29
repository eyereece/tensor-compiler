; ModuleID = 'LLVMDialectModule'
source_filename = "LLVMDialectModule"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i8:8:32-i16:16:32-i64:64-i128:128-n32:64-S128-Fn32"
target triple = "aarch64-unknown-linux-gnu"

declare ptr @malloc(i64)

define { ptr, ptr, i64, [2 x i64], [2 x i64] } @main(ptr %0, ptr %1, i64 %2, i64 %3, i64 %4, i64 %5, i64 %6, ptr %7, ptr %8, i64 %9, i64 %10, i64 %11, i64 %12, i64 %13) {
  %15 = insertvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } poison, ptr %0, 0
  %16 = insertvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %15, ptr %1, 1
  %17 = insertvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %16, i64 %2, 2
  %18 = insertvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %17, i64 %3, 3, 0
  %19 = insertvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %18, i64 %5, 4, 0
  %20 = insertvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %19, i64 %4, 3, 1
  %21 = insertvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %20, i64 %6, 4, 1
  %22 = insertvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } poison, ptr %7, 0
  %23 = insertvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %22, ptr %8, 1
  %24 = insertvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %23, i64 %9, 2
  %25 = insertvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %24, i64 %10, 3, 0
  %26 = insertvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %25, i64 %12, 4, 0
  %27 = insertvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %26, i64 %11, 3, 1
  %28 = insertvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %27, i64 %13, 4, 1
  %29 = call ptr @malloc(i64 16777280)
  %30 = ptrtoint ptr %29 to i64
  %31 = add i64 %30, 63
  %32 = urem i64 %31, 64
  %33 = sub i64 %31, %32
  %34 = inttoptr i64 %33 to ptr
  %35 = insertvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } poison, ptr %29, 0
  %36 = insertvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %35, ptr %34, 1
  %37 = insertvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %36, i64 0, 2
  %38 = insertvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %37, i64 2048, 3, 0
  %39 = insertvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %38, i64 2048, 3, 1
  %40 = insertvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %39, i64 2048, 4, 0
  %41 = insertvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %40, i64 1, 4, 1
  %42 = call ptr @malloc(i64 16777280)
  %43 = ptrtoint ptr %42 to i64
  %44 = add i64 %43, 63
  %45 = urem i64 %44, 64
  %46 = sub i64 %44, %45
  %47 = inttoptr i64 %46 to ptr
  %48 = insertvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } poison, ptr %42, 0
  %49 = insertvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %48, ptr %47, 1
  %50 = insertvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %49, i64 0, 2
  %51 = insertvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %50, i64 2048, 3, 0
  %52 = insertvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %51, i64 2048, 3, 1
  %53 = insertvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %52, i64 2048, 4, 0
  %54 = insertvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %53, i64 1, 4, 1
  br label %55

55:                                               ; preds = %78, %14
  %56 = phi i64 [ 0, %14 ], [ %79, %78 ]
  %57 = icmp slt i64 %56, 2048
  br i1 %57, label %58, label %80

58:                                               ; preds = %55
  br label %59

59:                                               ; preds = %62, %58
  %60 = phi i64 [ 0, %58 ], [ %77, %62 ]
  %61 = icmp slt i64 %60, 2048
  br i1 %61, label %62, label %78

62:                                               ; preds = %59
  %63 = extractvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %28, 1
  %64 = extractvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %28, 2
  %65 = getelementptr float, ptr %63, i64 %64
  %66 = extractvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %28, 4, 0
  %67 = mul nuw nsw i64 %60, %66
  %68 = extractvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %28, 4, 1
  %69 = mul nuw nsw i64 %56, %68
  %70 = add nuw nsw i64 %67, %69
  %71 = getelementptr inbounds nuw float, ptr %65, i64 %70
  %72 = load float, ptr %71, align 4
  %73 = extractvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %54, 1
  %74 = mul nuw nsw i64 %56, 2048
  %75 = add nuw nsw i64 %74, %60
  %76 = getelementptr inbounds nuw float, ptr %73, i64 %75
  store float %72, ptr %76, align 4
  %77 = add i64 %60, 1
  br label %59

78:                                               ; preds = %59
  %79 = add i64 %56, 1
  br label %55

80:                                               ; preds = %55
  br label %81

81:                                               ; preds = %94, %80
  %82 = phi i64 [ 0, %80 ], [ %95, %94 ]
  %83 = icmp slt i64 %82, 2048
  br i1 %83, label %84, label %96

84:                                               ; preds = %81
  br label %85

85:                                               ; preds = %88, %84
  %86 = phi i64 [ 0, %84 ], [ %93, %88 ]
  %87 = icmp slt i64 %86, 2048
  br i1 %87, label %88, label %94

88:                                               ; preds = %85
  %89 = extractvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %41, 1
  %90 = mul nuw nsw i64 %82, 2048
  %91 = add nuw nsw i64 %90, %86
  %92 = getelementptr inbounds nuw float, ptr %89, i64 %91
  store float 0.000000e+00, ptr %92, align 4
  %93 = add i64 %86, 1
  br label %85

94:                                               ; preds = %85
  %95 = add i64 %82, 1
  br label %81

96:                                               ; preds = %81
  br label %97

97:                                               ; preds = %138, %96
  %98 = phi i64 [ 0, %96 ], [ %139, %138 ]
  %99 = icmp slt i64 %98, 2048
  br i1 %99, label %100, label %140

100:                                              ; preds = %97
  br label %101

101:                                              ; preds = %136, %100
  %102 = phi i64 [ 0, %100 ], [ %137, %136 ]
  %103 = icmp slt i64 %102, 2048
  br i1 %103, label %104, label %138

104:                                              ; preds = %101
  br label %105

105:                                              ; preds = %108, %104
  %106 = phi i64 [ 0, %104 ], [ %135, %108 ]
  %107 = icmp slt i64 %106, 2048
  br i1 %107, label %108, label %136

108:                                              ; preds = %105
  %109 = extractvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %21, 1
  %110 = extractvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %21, 2
  %111 = getelementptr float, ptr %109, i64 %110
  %112 = extractvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %21, 4, 0
  %113 = mul nuw nsw i64 %98, %112
  %114 = extractvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %21, 4, 1
  %115 = mul nuw nsw i64 %106, %114
  %116 = add nuw nsw i64 %113, %115
  %117 = getelementptr inbounds nuw float, ptr %111, i64 %116
  %118 = load float, ptr %117, align 4
  %119 = extractvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %54, 1
  %120 = mul nuw nsw i64 %102, 2048
  %121 = add nuw nsw i64 %120, %106
  %122 = getelementptr inbounds nuw float, ptr %119, i64 %121
  %123 = load float, ptr %122, align 4
  %124 = extractvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %41, 1
  %125 = mul nuw nsw i64 %98, 2048
  %126 = add nuw nsw i64 %125, %102
  %127 = getelementptr inbounds nuw float, ptr %124, i64 %126
  %128 = load float, ptr %127, align 4
  %129 = fmul float %118, %123
  %130 = fadd float %128, %129
  %131 = extractvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %41, 1
  %132 = mul nuw nsw i64 %98, 2048
  %133 = add nuw nsw i64 %132, %102
  %134 = getelementptr inbounds nuw float, ptr %131, i64 %133
  store float %130, ptr %134, align 4
  %135 = add i64 %106, 1
  br label %105

136:                                              ; preds = %105
  %137 = add i64 %102, 1
  br label %101

138:                                              ; preds = %101
  %139 = add i64 %98, 1
  br label %97

140:                                              ; preds = %97
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

