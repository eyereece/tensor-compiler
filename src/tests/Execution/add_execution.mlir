// RUN: %driver -emit=jit %s | FileCheck %s

module {
  func.func @main() -> tensor<2xf32> attributes {llvm.emit_c_interface} {
    // 1.0 + 3.0 = 4.0
    // 2.0 + 4.0 = 6.0
    %cst = arith.constant dense<[1.0, 2.0]> : tensor<2xf32>
    %cst_0 = arith.constant dense<[3.0, 4.0]> : tensor<2xf32>
    
    %0 = "dlc.add"(%cst, %cst_0) : (tensor<2xf32>, tensor<2xf32>) -> tensor<2xf32>
    return %0 : tensor<2xf32>
  }
}

// CHECK: Result Shape: [ 2 ]
// CHECK: Data: 4.0{{0*}} 6.0{{0*}}