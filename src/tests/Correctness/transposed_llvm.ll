; ModuleID = 'LLVMDialectModule'
source_filename = "LLVMDialectModule"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i8:8:32-i16:16:32-i64:64-i128:128-n32:64-S128-Fn32"
target triple = "aarch64-unknown-linux-gnu"

declare ptr @malloc(i64)

define { ptr, ptr, i64, [2 x i64], [2 x i64] } @main(ptr %0, ptr %1, i64 %2, i64 %3, i64 %4, i64 %5, i64 %6, ptr %7, ptr %8, i64 %9, i64 %10, i64 %11, i64 %12, i64 %13, ptr %14, ptr %15, i64 %16, i64 %17, i64 %18, i64 %19, i64 %20) {
  %22 = insertvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } poison, ptr %14, 0
  %23 = insertvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %22, ptr %15, 1
  %24 = insertvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %23, i64 %16, 2
  %25 = insertvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %24, i64 %17, 3, 0
  %26 = insertvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %25, i64 %19, 4, 0
  %27 = insertvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %26, i64 %18, 3, 1
  %28 = insertvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %27, i64 %20, 4, 1
  %29 = insertvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } poison, ptr %0, 0
  %30 = insertvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %29, ptr %1, 1
  %31 = insertvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %30, i64 %2, 2
  %32 = insertvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %31, i64 %3, 3, 0
  %33 = insertvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %32, i64 %5, 4, 0
  %34 = insertvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %33, i64 %4, 3, 1
  %35 = insertvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %34, i64 %6, 4, 1
  %36 = insertvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } poison, ptr %7, 0
  %37 = insertvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %36, ptr %8, 1
  %38 = insertvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %37, i64 %9, 2
  %39 = insertvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %38, i64 %10, 3, 0
  %40 = insertvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %39, i64 %12, 4, 0
  %41 = insertvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %40, i64 %11, 3, 1
  %42 = insertvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %41, i64 %13, 4, 1
  %43 = call ptr @malloc(i64 16777280)
  %44 = ptrtoint ptr %43 to i64
  %45 = add i64 %44, 63
  %46 = urem i64 %45, 64
  %47 = sub i64 %45, %46
  %48 = inttoptr i64 %47 to ptr
  %49 = insertvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } poison, ptr %43, 0
  %50 = insertvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %49, ptr %48, 1
  %51 = insertvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %50, i64 0, 2
  %52 = insertvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %51, i64 2048, 3, 0
  %53 = insertvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %52, i64 2048, 3, 1
  %54 = insertvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %53, i64 2048, 4, 0
  %55 = insertvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %54, i64 1, 4, 1
  %56 = call ptr @malloc(i64 16777280)
  %57 = ptrtoint ptr %56 to i64
  %58 = add i64 %57, 63
  %59 = urem i64 %58, 64
  %60 = sub i64 %58, %59
  %61 = inttoptr i64 %60 to ptr
  %62 = insertvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } poison, ptr %56, 0
  %63 = insertvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %62, ptr %61, 1
  %64 = insertvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %63, i64 0, 2
  %65 = insertvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %64, i64 2048, 3, 0
  %66 = insertvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %65, i64 2048, 3, 1
  %67 = insertvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %66, i64 2048, 4, 0
  %68 = insertvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %67, i64 1, 4, 1
  br label %69

69:                                               ; preds = %82, %21
  %70 = phi i64 [ 0, %21 ], [ %83, %82 ]
  %71 = icmp slt i64 %70, 2048
  br i1 %71, label %72, label %84

72:                                               ; preds = %69
  br label %73

73:                                               ; preds = %76, %72
  %74 = phi i64 [ 0, %72 ], [ %81, %76 ]
  %75 = icmp slt i64 %74, 2048
  br i1 %75, label %76, label %82

76:                                               ; preds = %73
  %77 = extractvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %68, 1
  %78 = mul nuw nsw i64 %70, 2048
  %79 = add nuw nsw i64 %78, %74
  %80 = getelementptr inbounds nuw float, ptr %77, i64 %79
  store float 0.000000e+00, ptr %80, align 4
  %81 = add i64 %74, 1
  br label %73

82:                                               ; preds = %73
  %83 = add i64 %70, 1
  br label %69

84:                                               ; preds = %69
  br label %85

85:                                               ; preds = %108, %84
  %86 = phi i64 [ 0, %84 ], [ %109, %108 ]
  %87 = icmp slt i64 %86, 2048
  br i1 %87, label %88, label %110

88:                                               ; preds = %85
  br label %89

89:                                               ; preds = %92, %88
  %90 = phi i64 [ 0, %88 ], [ %107, %92 ]
  %91 = icmp slt i64 %90, 2048
  br i1 %91, label %92, label %108

92:                                               ; preds = %89
  %93 = extractvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %42, 1
  %94 = extractvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %42, 2
  %95 = getelementptr float, ptr %93, i64 %94
  %96 = extractvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %42, 4, 0
  %97 = mul nuw nsw i64 %90, %96
  %98 = extractvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %42, 4, 1
  %99 = mul nuw nsw i64 %86, %98
  %100 = add nuw nsw i64 %97, %99
  %101 = getelementptr inbounds nuw float, ptr %95, i64 %100
  %102 = load float, ptr %101, align 4
  %103 = extractvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %55, 1
  %104 = mul nuw nsw i64 %86, 2048
  %105 = add nuw nsw i64 %104, %90
  %106 = getelementptr inbounds nuw float, ptr %103, i64 %105
  store float %102, ptr %106, align 4
  %107 = add i64 %90, 1
  br label %89

108:                                              ; preds = %89
  %109 = add i64 %86, 1
  br label %85

110:                                              ; preds = %85
  br label %111

111:                                              ; preds = %152, %110
  %112 = phi i64 [ 0, %110 ], [ %153, %152 ]
  %113 = icmp slt i64 %112, 2048
  br i1 %113, label %114, label %154

114:                                              ; preds = %111
  br label %115

115:                                              ; preds = %150, %114
  %116 = phi i64 [ 0, %114 ], [ %151, %150 ]
  %117 = icmp slt i64 %116, 2048
  br i1 %117, label %118, label %152

118:                                              ; preds = %115
  br label %119

119:                                              ; preds = %122, %118
  %120 = phi i64 [ 0, %118 ], [ %149, %122 ]
  %121 = icmp slt i64 %120, 2048
  br i1 %121, label %122, label %150

122:                                              ; preds = %119
  %123 = extractvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %35, 1
  %124 = extractvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %35, 2
  %125 = getelementptr float, ptr %123, i64 %124
  %126 = extractvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %35, 4, 0
  %127 = mul nuw nsw i64 %112, %126
  %128 = extractvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %35, 4, 1
  %129 = mul nuw nsw i64 %120, %128
  %130 = add nuw nsw i64 %127, %129
  %131 = getelementptr inbounds nuw float, ptr %125, i64 %130
  %132 = load float, ptr %131, align 4
  %133 = extractvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %55, 1
  %134 = mul nuw nsw i64 %116, 2048
  %135 = add nuw nsw i64 %134, %120
  %136 = getelementptr inbounds nuw float, ptr %133, i64 %135
  %137 = load float, ptr %136, align 4
  %138 = extractvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %68, 1
  %139 = mul nuw nsw i64 %112, 2048
  %140 = add nuw nsw i64 %139, %116
  %141 = getelementptr inbounds nuw float, ptr %138, i64 %140
  %142 = load float, ptr %141, align 4
  %143 = fmul float %132, %137
  %144 = fadd float %142, %143
  %145 = extractvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %68, 1
  %146 = mul nuw nsw i64 %112, 2048
  %147 = add nuw nsw i64 %146, %116
  %148 = getelementptr inbounds nuw float, ptr %145, i64 %147
  store float %144, ptr %148, align 4
  %149 = add i64 %120, 1
  br label %119

150:                                              ; preds = %119
  %151 = add i64 %116, 1
  br label %115

152:                                              ; preds = %115
  %153 = add i64 %112, 1
  br label %111

154:                                              ; preds = %111
  br label %155

155:                                              ; preds = %184, %154
  %156 = phi i64 [ 0, %154 ], [ %185, %184 ]
  %157 = icmp slt i64 %156, 2048
  br i1 %157, label %158, label %186

158:                                              ; preds = %155
  br label %159

159:                                              ; preds = %162, %158
  %160 = phi i64 [ 0, %158 ], [ %183, %162 ]
  %161 = icmp slt i64 %160, 2048
  br i1 %161, label %162, label %184

162:                                              ; preds = %159
  %163 = extractvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %68, 1
  %164 = mul nuw nsw i64 %156, 2048
  %165 = add nuw nsw i64 %164, %160
  %166 = getelementptr inbounds nuw float, ptr %163, i64 %165
  %167 = load float, ptr %166, align 4
  %168 = extractvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %28, 1
  %169 = extractvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %28, 2
  %170 = getelementptr float, ptr %168, i64 %169
  %171 = extractvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %28, 4, 0
  %172 = mul nuw nsw i64 %156, %171
  %173 = extractvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %28, 4, 1
  %174 = mul nuw nsw i64 %160, %173
  %175 = add nuw nsw i64 %172, %174
  %176 = getelementptr inbounds nuw float, ptr %170, i64 %175
  %177 = load float, ptr %176, align 4
  %178 = fadd float %167, %177
  %179 = extractvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %55, 1
  %180 = mul nuw nsw i64 %156, 2048
  %181 = add nuw nsw i64 %180, %160
  %182 = getelementptr inbounds nuw float, ptr %179, i64 %181
  store float %178, ptr %182, align 4
  %183 = add i64 %160, 1
  br label %159

184:                                              ; preds = %159
  %185 = add i64 %156, 1
  br label %155

186:                                              ; preds = %155
  br label %187

187:                                              ; preds = %206, %186
  %188 = phi i64 [ 0, %186 ], [ %207, %206 ]
  %189 = icmp slt i64 %188, 2048
  br i1 %189, label %190, label %208

190:                                              ; preds = %187
  br label %191

191:                                              ; preds = %194, %190
  %192 = phi i64 [ 0, %190 ], [ %205, %194 ]
  %193 = icmp slt i64 %192, 2048
  br i1 %193, label %194, label %206

194:                                              ; preds = %191
  %195 = extractvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %55, 1
  %196 = mul nuw nsw i64 %188, 2048
  %197 = add nuw nsw i64 %196, %192
  %198 = getelementptr inbounds nuw float, ptr %195, i64 %197
  %199 = load float, ptr %198, align 4
  %200 = call float @llvm.maxnum.f32(float %199, float 0.000000e+00)
  %201 = extractvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %55, 1
  %202 = mul nuw nsw i64 %188, 2048
  %203 = add nuw nsw i64 %202, %192
  %204 = getelementptr inbounds nuw float, ptr %201, i64 %203
  store float %200, ptr %204, align 4
  %205 = add i64 %192, 1
  br label %191

206:                                              ; preds = %191
  %207 = add i64 %188, 1
  br label %187

208:                                              ; preds = %187
  ret { ptr, ptr, i64, [2 x i64], [2 x i64] } %55
}

define void @_mlir_ciface_main(ptr %0, ptr %1, ptr %2, ptr %3) {
  %5 = load { ptr, ptr, i64, [2 x i64], [2 x i64] }, ptr %1, align 8
  %6 = extractvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %5, 0
  %7 = extractvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %5, 1
  %8 = extractvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %5, 2
  %9 = extractvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %5, 3, 0
  %10 = extractvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %5, 3, 1
  %11 = extractvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %5, 4, 0
  %12 = extractvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %5, 4, 1
  %13 = load { ptr, ptr, i64, [2 x i64], [2 x i64] }, ptr %2, align 8
  %14 = extractvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %13, 0
  %15 = extractvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %13, 1
  %16 = extractvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %13, 2
  %17 = extractvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %13, 3, 0
  %18 = extractvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %13, 3, 1
  %19 = extractvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %13, 4, 0
  %20 = extractvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %13, 4, 1
  %21 = load { ptr, ptr, i64, [2 x i64], [2 x i64] }, ptr %3, align 8
  %22 = extractvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %21, 0
  %23 = extractvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %21, 1
  %24 = extractvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %21, 2
  %25 = extractvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %21, 3, 0
  %26 = extractvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %21, 3, 1
  %27 = extractvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %21, 4, 0
  %28 = extractvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %21, 4, 1
  %29 = call { ptr, ptr, i64, [2 x i64], [2 x i64] } @main(ptr %6, ptr %7, i64 %8, i64 %9, i64 %10, i64 %11, i64 %12, ptr %14, ptr %15, i64 %16, i64 %17, i64 %18, i64 %19, i64 %20, ptr %22, ptr %23, i64 %24, i64 %25, i64 %26, i64 %27, i64 %28)
  store { ptr, ptr, i64, [2 x i64], [2 x i64] } %29, ptr %0, align 8
  ret void
}

; Function Attrs: nocallback nocreateundeforpoison nofree nosync nounwind speculatable willreturn memory(none)
declare float @llvm.maxnum.f32(float, float) #0

attributes #0 = { nocallback nocreateundeforpoison nofree nosync nounwind speculatable willreturn memory(none) }

!llvm.module.flags = !{!0}

!0 = !{i32 2, !"Debug Info Version", i32 3}

