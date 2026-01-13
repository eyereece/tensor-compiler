// RUN: %driver -emit=mlir-tensor %s | FileCheck %s
module {
  func.func @main() -> tensor<2xf32> attributes {llvm.emit_c_interface} {
    %0 = "dlc.constant"() <{value = dense<[1.000000e+00, 2.000000e+00]> : tensor<2xf32>}> : () -> tensor<2xf32>
    %1 = "dlc.constant"() <{value = dense<[3.000000e+00, 4.000000e+00]> : tensor<2xf32>}> : () -> tensor<2xf32>
    %2 = "dlc.add"(%0, %1) : (tensor<2xf32>, tensor<2xf32>) -> tensor<2xf32>
    return %2 : tensor<2xf32>
  }
}
// CHECK: linalg.add ins(%{{.*}}, %{{.*}} : tensor<2xf32>, tensor<2xf32>)