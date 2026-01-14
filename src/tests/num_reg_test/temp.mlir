
// RUN: python3 verify_add.py
func.func @main() -> tensor<2xf32> attributes {llvm.emit_c_interface} {
  %0 = arith.constant dense<[0.7648205757141113, 0.46996545791625977]> : tensor<2xf32>
  %1 = arith.constant dense<[0.040904492139816284, 0.36252284049987793]> : tensor<2xf32>
  %2 = "dlc.add"(%0, %1) : (tensor<2xf32>, tensor<2xf32>) -> tensor<2xf32>
  return %2 : tensor<2xf32>
}
