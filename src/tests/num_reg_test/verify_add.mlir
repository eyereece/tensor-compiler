
// RUN: python3 verify_add.py
func.func @main() -> tensor<2xf32> attributes {llvm.emit_c_interface} {
  %0 = arith.constant dense<[0.7943139672279358, 0.8351204991340637]> : tensor<2xf32>
  %1 = arith.constant dense<[0.8729273676872253, 0.8073111176490784]> : tensor<2xf32>
  %2 = "dlc.add"(%0, %1) : (tensor<2xf32>, tensor<2xf32>) -> tensor<2xf32>
  return %2 : tensor<2xf32>
}
