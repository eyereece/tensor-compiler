; ModuleID = 'LLVMDialectModule'
source_filename = "LLVMDialectModule"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i8:8:32-i16:16:32-i64:64-i128:128-n32:64-S128-Fn32"
target triple = "aarch64-unknown-linux-gnu"

declare void @memrefCopy(i64, ptr, ptr)

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

69:                                               ; preds = %92, %21
  %70 = phi i64 [ 0, %21 ], [ %93, %92 ]
  %71 = icmp slt i64 %70, 2048
  br i1 %71, label %72, label %94

72:                                               ; preds = %69
  br label %73

73:                                               ; preds = %76, %72
  %74 = phi i64 [ 0, %72 ], [ %91, %76 ]
  %75 = icmp slt i64 %74, 2048
  br i1 %75, label %76, label %92

76:                                               ; preds = %73
  %77 = extractvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %42, 1
  %78 = extractvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %42, 2
  %79 = getelementptr float, ptr %77, i64 %78
  %80 = extractvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %42, 4, 0
  %81 = mul nuw nsw i64 %74, %80
  %82 = extractvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %42, 4, 1
  %83 = mul nuw nsw i64 %70, %82
  %84 = add nuw nsw i64 %81, %83
  %85 = getelementptr inbounds nuw float, ptr %79, i64 %84
  %86 = load float, ptr %85, align 4
  %87 = extractvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %68, 1
  %88 = mul nuw nsw i64 %70, 2048
  %89 = add nuw nsw i64 %88, %74
  %90 = getelementptr inbounds nuw float, ptr %87, i64 %89
  store float %86, ptr %90, align 4
  %91 = add i64 %74, 1
  br label %73

92:                                               ; preds = %73
  %93 = add i64 %70, 1
  br label %69

94:                                               ; preds = %69
  br label %95

95:                                               ; preds = %329, %94
  %96 = phi i64 [ 0, %94 ], [ %330, %329 ]
  %97 = icmp slt i64 %96, 2048
  br i1 %97, label %98, label %331

98:                                               ; preds = %95
  br label %99

99:                                               ; preds = %320, %98
  %100 = phi i64 [ 0, %98 ], [ %328, %320 ]
  %101 = icmp slt i64 %100, 2048
  br i1 %101, label %102, label %329

102:                                              ; preds = %99
  %103 = call ptr @malloc(i64 16448)
  %104 = ptrtoint ptr %103 to i64
  %105 = add i64 %104, 63
  %106 = urem i64 %105, 64
  %107 = sub i64 %105, %106
  %108 = inttoptr i64 %107 to ptr
  %109 = insertvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } poison, ptr %103, 0
  %110 = insertvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %109, ptr %108, 1
  %111 = insertvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %110, i64 0, 2
  %112 = insertvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %111, i64 64, 3, 0
  %113 = insertvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %112, i64 64, 3, 1
  %114 = insertvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %113, i64 64, 4, 0
  %115 = insertvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %114, i64 1, 4, 1
  %116 = call ptr @malloc(i64 16448)
  %117 = ptrtoint ptr %116 to i64
  %118 = add i64 %117, 63
  %119 = urem i64 %118, 64
  %120 = sub i64 %118, %119
  %121 = inttoptr i64 %120 to ptr
  %122 = insertvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } poison, ptr %116, 0
  %123 = insertvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %122, ptr %121, 1
  %124 = insertvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %123, i64 0, 2
  %125 = insertvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %124, i64 64, 3, 0
  %126 = insertvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %125, i64 64, 3, 1
  %127 = insertvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %126, i64 64, 4, 0
  %128 = insertvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %127, i64 1, 4, 1
  br label %129

129:                                              ; preds = %142, %102
  %130 = phi i64 [ 0, %102 ], [ %143, %142 ]
  %131 = icmp slt i64 %130, 64
  br i1 %131, label %132, label %144

132:                                              ; preds = %129
  br label %133

133:                                              ; preds = %136, %132
  %134 = phi i64 [ 0, %132 ], [ %141, %136 ]
  %135 = icmp slt i64 %134, 64
  br i1 %135, label %136, label %142

136:                                              ; preds = %133
  %137 = extractvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %128, 1
  %138 = mul nuw nsw i64 %130, 64
  %139 = add nuw nsw i64 %138, %134
  %140 = getelementptr inbounds nuw float, ptr %137, i64 %139
  store float 0.000000e+00, ptr %140, align 4
  %141 = add i64 %134, 1
  br label %133

142:                                              ; preds = %133
  %143 = add i64 %130, 1
  br label %129

144:                                              ; preds = %129
  br label %145

145:                                              ; preds = %228, %144
  %146 = phi i64 [ 0, %144 ], [ %229, %228 ]
  %147 = icmp slt i64 %146, 2048
  br i1 %147, label %148, label %230

148:                                              ; preds = %145
  %149 = extractvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %35, 0
  %150 = extractvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %35, 1
  %151 = insertvalue { ptr, ptr, i64 } poison, ptr %149, 0
  %152 = insertvalue { ptr, ptr, i64 } %151, ptr %150, 1
  %153 = insertvalue { ptr, ptr, i64 } %152, i64 0, 2
  %154 = extractvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %35, 2
  %155 = extractvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %35, 3, 0
  %156 = extractvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %35, 3, 1
  %157 = extractvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %35, 4, 0
  %158 = extractvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %35, 4, 1
  %159 = mul nsw i64 %96, %157
  %160 = add i64 %154, %159
  %161 = mul nsw i64 %146, %158
  %162 = add i64 %160, %161
  %163 = extractvalue { ptr, ptr, i64 } %153, 0
  %164 = extractvalue { ptr, ptr, i64 } %153, 1
  %165 = insertvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } poison, ptr %163, 0
  %166 = insertvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %165, ptr %164, 1
  %167 = insertvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %166, i64 %162, 2
  %168 = insertvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %167, i64 64, 3, 0
  %169 = insertvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %168, i64 %157, 4, 0
  %170 = insertvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %169, i64 64, 3, 1
  %171 = insertvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %170, i64 %158, 4, 1
  %172 = mul nsw i64 %100, 2048
  %173 = add i64 %172, %146
  %174 = extractvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %68, 0
  %175 = extractvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %68, 1
  %176 = insertvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } poison, ptr %174, 0
  %177 = insertvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %176, ptr %175, 1
  %178 = insertvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %177, i64 %173, 2
  %179 = insertvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %178, i64 64, 3, 0
  %180 = insertvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %179, i64 2048, 4, 0
  %181 = insertvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %180, i64 64, 3, 1
  %182 = insertvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %181, i64 1, 4, 1
  br label %183

183:                                              ; preds = %226, %148
  %184 = phi i64 [ 0, %148 ], [ %227, %226 ]
  %185 = icmp slt i64 %184, 64
  br i1 %185, label %186, label %228

186:                                              ; preds = %183
  br label %187

187:                                              ; preds = %224, %186
  %188 = phi i64 [ 0, %186 ], [ %225, %224 ]
  %189 = icmp slt i64 %188, 64
  br i1 %189, label %190, label %226

190:                                              ; preds = %187
  br label %191

191:                                              ; preds = %194, %190
  %192 = phi i64 [ 0, %190 ], [ %223, %194 ]
  %193 = icmp slt i64 %192, 64
  br i1 %193, label %194, label %224

194:                                              ; preds = %191
  %195 = extractvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %171, 1
  %196 = extractvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %171, 2
  %197 = getelementptr float, ptr %195, i64 %196
  %198 = extractvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %171, 4, 0
  %199 = mul nuw nsw i64 %184, %198
  %200 = extractvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %171, 4, 1
  %201 = mul nuw nsw i64 %192, %200
  %202 = add nuw nsw i64 %199, %201
  %203 = getelementptr inbounds nuw float, ptr %197, i64 %202
  %204 = load float, ptr %203, align 4
  %205 = extractvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %182, 1
  %206 = extractvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %182, 2
  %207 = getelementptr float, ptr %205, i64 %206
  %208 = mul nuw nsw i64 %188, 2048
  %209 = add nuw nsw i64 %208, %192
  %210 = getelementptr inbounds nuw float, ptr %207, i64 %209
  %211 = load float, ptr %210, align 4
  %212 = extractvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %128, 1
  %213 = mul nuw nsw i64 %184, 64
  %214 = add nuw nsw i64 %213, %188
  %215 = getelementptr inbounds nuw float, ptr %212, i64 %214
  %216 = load float, ptr %215, align 4
  %217 = fmul float %204, %211
  %218 = fadd float %216, %217
  %219 = extractvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %128, 1
  %220 = mul nuw nsw i64 %184, 64
  %221 = add nuw nsw i64 %220, %188
  %222 = getelementptr inbounds nuw float, ptr %219, i64 %221
  store float %218, ptr %222, align 4
  %223 = add i64 %192, 1
  br label %191

224:                                              ; preds = %191
  %225 = add i64 %188, 1
  br label %187

226:                                              ; preds = %187
  %227 = add i64 %184, 1
  br label %183

228:                                              ; preds = %183
  %229 = add i64 %146, 64
  br label %145

230:                                              ; preds = %145
  %231 = extractvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %28, 0
  %232 = extractvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %28, 1
  %233 = insertvalue { ptr, ptr, i64 } poison, ptr %231, 0
  %234 = insertvalue { ptr, ptr, i64 } %233, ptr %232, 1
  %235 = insertvalue { ptr, ptr, i64 } %234, i64 0, 2
  %236 = extractvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %28, 2
  %237 = extractvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %28, 3, 0
  %238 = extractvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %28, 3, 1
  %239 = extractvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %28, 4, 0
  %240 = extractvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %28, 4, 1
  %241 = mul nsw i64 %96, %239
  %242 = add i64 %236, %241
  %243 = mul nsw i64 %100, %240
  %244 = add i64 %242, %243
  %245 = extractvalue { ptr, ptr, i64 } %235, 0
  %246 = extractvalue { ptr, ptr, i64 } %235, 1
  %247 = insertvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } poison, ptr %245, 0
  %248 = insertvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %247, ptr %246, 1
  %249 = insertvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %248, i64 %244, 2
  %250 = insertvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %249, i64 64, 3, 0
  %251 = insertvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %250, i64 %239, 4, 0
  %252 = insertvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %251, i64 64, 3, 1
  %253 = insertvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %252, i64 %240, 4, 1
  br label %254

254:                                              ; preds = %283, %230
  %255 = phi i64 [ 0, %230 ], [ %284, %283 ]
  %256 = icmp slt i64 %255, 64
  br i1 %256, label %257, label %285

257:                                              ; preds = %254
  br label %258

258:                                              ; preds = %261, %257
  %259 = phi i64 [ 0, %257 ], [ %282, %261 ]
  %260 = icmp slt i64 %259, 64
  br i1 %260, label %261, label %283

261:                                              ; preds = %258
  %262 = extractvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %128, 1
  %263 = mul nuw nsw i64 %255, 64
  %264 = add nuw nsw i64 %263, %259
  %265 = getelementptr inbounds nuw float, ptr %262, i64 %264
  %266 = load float, ptr %265, align 4
  %267 = extractvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %253, 1
  %268 = extractvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %253, 2
  %269 = getelementptr float, ptr %267, i64 %268
  %270 = extractvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %253, 4, 0
  %271 = mul nuw nsw i64 %255, %270
  %272 = extractvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %253, 4, 1
  %273 = mul nuw nsw i64 %259, %272
  %274 = add nuw nsw i64 %271, %273
  %275 = getelementptr inbounds nuw float, ptr %269, i64 %274
  %276 = load float, ptr %275, align 4
  %277 = fadd float %266, %276
  %278 = extractvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %115, 1
  %279 = mul nuw nsw i64 %255, 64
  %280 = add nuw nsw i64 %279, %259
  %281 = getelementptr inbounds nuw float, ptr %278, i64 %280
  store float %277, ptr %281, align 4
  %282 = add i64 %259, 1
  br label %258

283:                                              ; preds = %258
  %284 = add i64 %255, 1
  br label %254

285:                                              ; preds = %254
  %286 = mul nsw i64 %96, 2048
  %287 = add i64 %286, %100
  %288 = extractvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %55, 0
  %289 = extractvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %55, 1
  %290 = insertvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } poison, ptr %288, 0
  %291 = insertvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %290, ptr %289, 1
  %292 = insertvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %291, i64 %287, 2
  %293 = insertvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %292, i64 64, 3, 0
  %294 = insertvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %293, i64 2048, 4, 0
  %295 = insertvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %294, i64 64, 3, 1
  %296 = insertvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %295, i64 1, 4, 1
  br label %297

297:                                              ; preds = %318, %285
  %298 = phi i64 [ 0, %285 ], [ %319, %318 ]
  %299 = icmp slt i64 %298, 64
  br i1 %299, label %300, label %320

300:                                              ; preds = %297
  br label %301

301:                                              ; preds = %304, %300
  %302 = phi i64 [ 0, %300 ], [ %317, %304 ]
  %303 = icmp slt i64 %302, 64
  br i1 %303, label %304, label %318

304:                                              ; preds = %301
  %305 = extractvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %115, 1
  %306 = mul nuw nsw i64 %298, 64
  %307 = add nuw nsw i64 %306, %302
  %308 = getelementptr inbounds nuw float, ptr %305, i64 %307
  %309 = load float, ptr %308, align 4
  %310 = call float @llvm.maxnum.f32(float %309, float 0.000000e+00)
  %311 = extractvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %296, 1
  %312 = extractvalue { ptr, ptr, i64, [2 x i64], [2 x i64] } %296, 2
  %313 = getelementptr float, ptr %311, i64 %312
  %314 = mul nuw nsw i64 %298, 2048
  %315 = add nuw nsw i64 %314, %302
  %316 = getelementptr inbounds nuw float, ptr %313, i64 %315
  store float %310, ptr %316, align 4
  %317 = add i64 %302, 1
  br label %301

318:                                              ; preds = %301
  %319 = add i64 %298, 1
  br label %297

320:                                              ; preds = %297
  %321 = call ptr @llvm.stacksave.p0()
  %322 = alloca { ptr, ptr, i64, [2 x i64], [2 x i64] }, i64 1, align 8
  store { ptr, ptr, i64, [2 x i64], [2 x i64] } %296, ptr %322, align 8
  %323 = insertvalue { i64, ptr } { i64 2, ptr poison }, ptr %322, 1
  %324 = alloca { ptr, ptr, i64, [2 x i64], [2 x i64] }, i64 1, align 8
  store { ptr, ptr, i64, [2 x i64], [2 x i64] } %296, ptr %324, align 8
  %325 = insertvalue { i64, ptr } { i64 2, ptr poison }, ptr %324, 1
  %326 = alloca { i64, ptr }, i64 1, align 8
  store { i64, ptr } %323, ptr %326, align 8
  %327 = alloca { i64, ptr }, i64 1, align 8
  store { i64, ptr } %325, ptr %327, align 8
  call void @memrefCopy(i64 4, ptr %326, ptr %327)
  call void @llvm.stackrestore.p0(ptr %321)
  %328 = add i64 %100, 64
  br label %99

329:                                              ; preds = %99
  %330 = add i64 %96, 64
  br label %95

331:                                              ; preds = %95
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

; Function Attrs: nocallback nofree nosync nounwind willreturn
declare ptr @llvm.stacksave.p0() #0

; Function Attrs: nocallback nofree nosync nounwind willreturn
declare void @llvm.stackrestore.p0(ptr) #0

; Function Attrs: nocallback nocreateundeforpoison nofree nosync nounwind speculatable willreturn memory(none)
declare float @llvm.maxnum.f32(float, float) #1

attributes #0 = { nocallback nofree nosync nounwind willreturn }
attributes #1 = { nocallback nocreateundeforpoison nofree nosync nounwind speculatable willreturn memory(none) }

!llvm.module.flags = !{!0}

!0 = !{i32 2, !"Debug Info Version", i32 3}

