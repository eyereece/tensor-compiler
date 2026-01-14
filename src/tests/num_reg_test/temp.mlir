
// RUN: python3 verify_add.py
func.func @main() -> tensor<2xf32> attributes {llvm.emit_c_interface} {
  %0 = arith.constant dense<[0.02657179720699787, 0.7639251947402954]> : tensor<2xf32>
  %1 = arith.constant dense<[0.859627366065979, 0.6228095889091492]> : tensor<2xf32>
  %2 = "dlc.add"(%0, %1) : (tensor<2xf32>, tensor<2xf32>) -> tensor<2xf32>
  return %2 : tensor<2xf32>
}
