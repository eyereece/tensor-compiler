// RUN: not ../../build/driver -emit=mlir %s 2>&1 | FileCheck %s
module {
  func.func @main() {
    // Parser sees: dense attribute is valid (3 elements for tensor<3>)
    // Verifier sees: Attribute (tensor<3>) does not match Result (tensor<2>)
    %0 = "dlc.constant"() {value = dense<[1.0, 2.0, 3.0]> : tensor<3xf32>} : () -> tensor<2xf32> loc("bad_const")
    return
  }
}

// CHECK: error: 'dlc.constant' op number of elements in 'value' attribute