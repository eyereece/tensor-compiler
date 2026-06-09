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

111:                                              ; preds = %221, %110
  %112 = phi i64 [ 0, %110 ], [ %222, %221 ]
  %113 = icmp slt i64 %112, 2048
  br i1 %113, label %114, label %223

114:                                              ; preds = %111
  br label %115

115:                                              ; preds = %219, %114
  %116 = phi i64 [ 0, %114 ], [ %220, %219 ]
  %117 = icmp slt i64 %116, 2048
  br i1 %117, label %118, label %221

118:                                              ; preds = %115
  br label %119

119:                                              ; preds = %217, %118
  %120 = phi i64 [ 0, %118 ], [ %218, %217 ]
  %121 = icmp slt i64 %120, 2048
  br i1 %121, label %122, label %219

122:                                              ; preds = %119
  %123 = extractvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %35, 0
  %124 = extractvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %35, 1
  %125 = insertvalue { ptr, ptr, i64 } poison, ptr %123, 0
  %126 = insertvalue { ptr, ptr, i64 } %125, ptr %124, 1
  %127 = insertvalue { ptr, ptr, i64 } %126, i64 0, 2
  %128 = extractvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %35, 2
  %129 = extractvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %35, 3, 0
  %130 = extractvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %35, 3, 1
  %131 = extractvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %35, 4, 0
  %132 = extractvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %35, 4, 1
  %133 = mul nsw i64 %112, %131
  %134 = add i64 %128, %133
  %135 = mul nsw i64 %120, %132
  %136 = add i64 %134, %135
  %137 = extractvalue { ptr, ptr, i64 } %127, 0
  %138 = extractvalue { ptr, ptr, i64 } %127, 1
  %139 = insertvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } poison, ptr %137, 0
  %140 = insertvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %139, ptr %138, 1
  %141 = insertvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %140, i64 %136, 2
  %142 = insertvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %141, i64 64, 3, 0
  %143 = insertvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %142, i64 %131, 4, 0
  %144 = insertvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %143, i64 64, 3, 1
  %145 = insertvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %144, i64 %132, 4, 1
  %146 = mul nsw i64 %116, 2048
  %147 = add i64 %146, %120
  %148 = extractvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %55, 0
  %149 = extractvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %55, 1
  %150 = insertvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } poison, ptr %148, 0
  %151 = insertvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %150, ptr %149, 1
  %152 = insertvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %151, i64 %147, 2
  %153 = insertvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %152, i64 64, 3, 0
  %154 = insertvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %153, i64 2048, 4, 0
  %155 = insertvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %154, i64 64, 3, 1
  %156 = insertvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %155, i64 1, 4, 1
  %157 = mul nsw i64 %112, 2048
  %158 = add i64 %157, %116
  %159 = extractvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %68, 0
  %160 = extractvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %68, 1
  %161 = insertvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } poison, ptr %159, 0
  %162 = insertvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %161, ptr %160, 1
  %163 = insertvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %162, i64 %158, 2
  %164 = insertvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %163, i64 64, 3, 0
  %165 = insertvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %164, i64 2048, 4, 0
  %166 = insertvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %165, i64 64, 3, 1
  %167 = insertvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %166, i64 1, 4, 1
  br label %168

168:                                              ; preds = %215, %122
  %169 = phi i64 [ 0, %122 ], [ %216, %215 ]
  %170 = icmp slt i64 %169, 64
  br i1 %170, label %171, label %217

171:                                              ; preds = %168
  br label %172

172:                                              ; preds = %213, %171
  %173 = phi i64 [ 0, %171 ], [ %214, %213 ]
  %174 = icmp slt i64 %173, 64
  br i1 %174, label %175, label %215

175:                                              ; preds = %172
  br label %176

176:                                              ; preds = %179, %175
  %177 = phi i64 [ 0, %175 ], [ %212, %179 ]
  %178 = icmp slt i64 %177, 64
  br i1 %178, label %179, label %213

179:                                              ; preds = %176
  %180 = extractvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %145, 1
  %181 = extractvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %145, 2
  %182 = getelementptr float, ptr %180, i64 %181
  %183 = extractvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %145, 4, 0
  %184 = mul nuw nsw i64 %169, %183
  %185 = extractvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %145, 4, 1
  %186 = mul nuw nsw i64 %177, %185
  %187 = add nuw nsw i64 %184, %186
  %188 = getelementptr inbounds nuw float, ptr %182, i64 %187
  %189 = load float, ptr %188, align 4
  %190 = extractvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %156, 1
  %191 = extractvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %156, 2
  %192 = getelementptr float, ptr %190, i64 %191
  %193 = mul nuw nsw i64 %173, 2048
  %194 = add nuw nsw i64 %193, %177
  %195 = getelementptr inbounds nuw float, ptr %192, i64 %194
  %196 = load float, ptr %195, align 4
  %197 = extractvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %167, 1
  %198 = extractvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %167, 2
  %199 = getelementptr float, ptr %197, i64 %198
  %200 = mul nuw nsw i64 %169, 2048
  %201 = add nuw nsw i64 %200, %173
  %202 = getelementptr inbounds nuw float, ptr %199, i64 %201
  %203 = load float, ptr %202, align 4
  %204 = fmul float %189, %196
  %205 = fadd float %203, %204
  %206 = extractvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %167, 1
  %207 = extractvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %167, 2
  %208 = getelementptr float, ptr %206, i64 %207
  %209 = mul nuw nsw i64 %169, 2048
  %210 = add nuw nsw i64 %209, %173
  %211 = getelementptr inbounds nuw float, ptr %208, i64 %210
  store float %205, ptr %211, align 4
  %212 = add i64 %177, 1
  br label %176

213:                                              ; preds = %176
  %214 = add i64 %173, 1
  br label %172

215:                                              ; preds = %172
  %216 = add i64 %169, 1
  br label %168

217:                                              ; preds = %168
  %218 = add i64 %120, 64
  br label %119

219:                                              ; preds = %119
  %220 = add i64 %116, 64
  br label %115

221:                                              ; preds = %115
  %222 = add i64 %112, 64
  br label %111

223:                                              ; preds = %111
  br label %224

224:                                              ; preds = %253, %223
  %225 = phi i64 [ 0, %223 ], [ %254, %253 ]
  %226 = icmp slt i64 %225, 2048
  br i1 %226, label %227, label %255

227:                                              ; preds = %224
  br label %228

228:                                              ; preds = %231, %227
  %229 = phi i64 [ 0, %227 ], [ %252, %231 ]
  %230 = icmp slt i64 %229, 2048
  br i1 %230, label %231, label %253

231:                                              ; preds = %228
  %232 = extractvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %68, 1
  %233 = mul nuw nsw i64 %225, 2048
  %234 = add nuw nsw i64 %233, %229
  %235 = getelementptr inbounds nuw float, ptr %232, i64 %234
  %236 = load float, ptr %235, align 4
  %237 = extractvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %28, 1
  %238 = extractvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %28, 2
  %239 = getelementptr float, ptr %237, i64 %238
  %240 = extractvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %28, 4, 0
  %241 = mul nuw nsw i64 %225, %240
  %242 = extractvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %28, 4, 1
  %243 = mul nuw nsw i64 %229, %242
  %244 = add nuw nsw i64 %241, %243
  %245 = getelementptr inbounds nuw float, ptr %239, i64 %244
  %246 = load float, ptr %245, align 4
  %247 = fadd float %236, %246
  %248 = extractvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %55, 1
  %249 = mul nuw nsw i64 %225, 2048
  %250 = add nuw nsw i64 %249, %229
  %251 = getelementptr inbounds nuw float, ptr %248, i64 %250
  store float %247, ptr %251, align 4
  %252 = add i64 %229, 1
  br label %228

253:                                              ; preds = %228
  %254 = add i64 %225, 1
  br label %224

255:                                              ; preds = %224
  br label %256

256:                                              ; preds = %275, %255
  %257 = phi i64 [ 0, %255 ], [ %276, %275 ]
  %258 = icmp slt i64 %257, 2048
  br i1 %258, label %259, label %277

259:                                              ; preds = %256
  br label %260

260:                                              ; preds = %263, %259
  %261 = phi i64 [ 0, %259 ], [ %274, %263 ]
  %262 = icmp slt i64 %261, 2048
  br i1 %262, label %263, label %275

263:                                              ; preds = %260
  %264 = extractvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %55, 1
  %265 = mul nuw nsw i64 %257, 2048
  %266 = add nuw nsw i64 %265, %261
  %267 = getelementptr inbounds nuw float, ptr %264, i64 %266
  %268 = load float, ptr %267, align 4
  %269 = call float @llvm.maxnum.f32(float %268, float 0.000000e+00)
  %270 = extractvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %55, 1
  %271 = mul nuw nsw i64 %257, 2048
  %272 = add nuw nsw i64 %271, %261
  %273 = getelementptr inbounds nuw float, ptr %270, i64 %272
  store float %269, ptr %273, align 4
  %274 = add i64 %261, 1
  br label %260

275:                                              ; preds = %260
  %276 = add i64 %257, 1
  br label %256

277:                                              ; preds = %256
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

