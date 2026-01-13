// RUN: %driver -emit=mlir-memref %s | FileCheck %s

// --- Test 1: Larger Shape (10 elements) ---
// This checks if the loop logic uses the shape from the tensor
// rather than a hardcoded '2'.
func.func @test_large_shape(%arg0: tensor<10xf32>, %arg1: tensor<10xf32>) -> tensor<10xf32> {
  %0 = tensor.empty() : tensor<10xf32>
  %1 = linalg.add ins(%arg0, %arg1 : tensor<10xf32>, tensor<10xf32>) outs(%0 : tensor<10xf32>) -> tensor<10xf32>
  return %1 : tensor<10xf32>
}
// CHECK-LABEL: func.func @test_large_shape
// CHECK: %[[C10:.*]] = arith.constant 10 : index
// CHECK: scf.for %{{.*}} = %{{.*}} to %[[C10]]

// --- Test 2: Different Data Type (Integer) ---
// This checks if the lowering switches from arith.addf (float) 
// to arith.addi (integer) automatically.
func.func @test_int_type(%arg0: tensor<2xi32>, %arg1: tensor<2xi32>) -> tensor<2xi32> {
  %0 = tensor.empty() : tensor<2xi32>
  %1 = linalg.add ins(%arg0, %arg1 : tensor<2xi32>, tensor<2xi32>) outs(%0 : tensor<2xi32>) -> tensor<2xi32>
  return %1 : tensor<2xi32>
}
// CHECK-LABEL: func.func @test_int_type
// CHECK: %[[LHS:.*]] = memref.load
// CHECK: %[[RHS:.*]] = memref.load
// CHECK: arith.addi %[[LHS]], %[[RHS]] : i32