// RUN: not ../../build/driver -emit=mlir %s 2>&1 | FileCheck %s
module {
  func.func @main() {
    %0 = "dlc.constant"() <{value = dense<[1.0, 2.0]> : tensor<2xf32>}> : () -> tensor<2xf32> loc("const1")
    %1 = "dlc.constant"() <{value = dense<[3.0]> : tensor<1xf32>}> : () -> tensor<1xf32> loc("const2")
    
    // ERROR: Adding a tensor<2> to a tensor<1>
    %2 = "dlc.add"(%0, %1) : (tensor<2xf32>, tensor<1xf32>) -> tensor<2xf32> loc("broken_add")
    
    return
  }
}

// CHECK: error: 'dlc.add' op requires LHS and RHS to have the same shape