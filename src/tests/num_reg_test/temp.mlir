
func.func @main() -> tensor<2xf32> attributes {llvm.emit_c_interface} {
  %0 = arith.constant dense<[0.81592857837677, 0.987368643283844]> : tensor<2xf32>
  %1 = arith.constant dense<[0.6627007722854614, 0.18295928835868835]> : tensor<2xf32>
  %2 = "dlc.add"(%0, %1) : (tensor<2xf32>, tensor<2xf32>) -> tensor<2xf32>
  return %2 : tensor<2xf32>
}
