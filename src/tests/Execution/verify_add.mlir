
// RUN: python3 verify_add.py
func.func @main() -> tensor<2xf32> attributes {llvm.emit_c_interface} {
  %0 = arith.constant dense<[0.9740450382232666, 0.3653790354728699]> : tensor<2xf32>
  %1 = arith.constant dense<[0.9835569262504578, 0.5245436429977417]> : tensor<2xf32>
  %2 = "dlc.add"(%0, %1) : (tensor<2xf32>, tensor<2xf32>) -> tensor<2xf32>
  return %2 : tensor<2xf32>
}
