// RUN: %driver -emit=jit %s | FileCheck %s

module {
  func.func @main() -> tensor<3xf32> attributes {llvm.emit_c_interface} {
    // Input: [1.0, -2.0, 3.0] -> Expected Output: [1.0, 0.0, 3.0]
    %input = arith.constant dense<[1.0, -2.0, 3.0]> : tensor<3xf32>
    %0 = "dlc.relu"(%input) : (tensor<3xf32>) -> tensor<3xf32>
    return %0 : tensor<3xf32>
  }
}
// CHECK: Result Shape: [ 3 ]
// CHECK: Data: 1.0{{0*}} 0.0{{0*}} 3.0{{0*}}