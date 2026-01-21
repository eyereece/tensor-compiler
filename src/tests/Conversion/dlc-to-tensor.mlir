// RUN: %driver -emit=mlir-memref %s | FileCheck %s

module {
  func.func @main() -> tensor<2xf32> attributes {llvm.emit_c_interface} {
    %cst = arith.constant dense<[1.000000e+00, 2.000000e+00]> : tensor<2xf32>
    %cst_0 = arith.constant dense<[3.000000e+00, 4.000000e+00]> : tensor<2xf32>
    %0 = tensor.empty() : tensor<2xf32>
    %1 = linalg.add ins(%cst, %cst_0 : tensor<2xf32>, tensor<2xf32>) outs(%0 : tensor<2xf32>) -> tensor<2xf32>
    return %1 : tensor<2xf32>
  }
}

// CHECK-LABEL: func.func @main
// CHECK-DAG:   %[[GLOBAL0:.*]] = memref.get_global @__constant_2xf32
// CHECK-DAG:   %[[GLOBAL1:.*]] = memref.get_global @__constant_2xf32_0
// CHECK:       %[[ALLOC:.*]] = memref.alloc()
// CHECK:       scf.for %[[IDX:.*]] = %{{.*}} to %{{.*}} step %{{.*}} {
// CHECK:         %[[VAL0:.*]] = memref.load %[[GLOBAL0]][%[[IDX]]]
// CHECK:         %[[VAL1:.*]] = memref.load %[[GLOBAL1]][%[[IDX]]]
// CHECK:         %[[SUM:.*]] = arith.addf %[[VAL0]], %[[VAL1]]
// CHECK:         memref.store %[[SUM]], %[[ALLOC]][%[[IDX]]]
// CHECK:       }
// CHECK:       return %[[ALLOC]]