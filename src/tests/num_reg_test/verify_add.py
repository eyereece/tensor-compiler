import numpy as np
import subprocess
import re

# 1. Generate random data
a = np.random.rand(2).astype(np.float32)
b = np.random.rand(2).astype(np.float32)
expected = a + b

# 2. Run compiler
# Generate temp.mlir
mlir_template = f"""
// RUN: python3 verify_add.py
func.func @main() -> tensor<2xf32> attributes {{llvm.emit_c_interface}} {{
  %0 = arith.constant dense<{a.tolist()}> : tensor<2xf32>
  %1 = arith.constant dense<{b.tolist()}> : tensor<2xf32>
  %2 = "dlc.add"(%0, %1) : (tensor<2xf32>, tensor<2xf32>) -> tensor<2xf32>
  return %2 : tensor<2xf32>
}}
"""
with open("temp.mlir", "w") as f: f.write(mlir_template)

# 3. Execute JIT
result = subprocess.check_output(["../../build/driver", "-emit=jit", "temp.mlir"]).decode()

# 4. Parse result using Regex
match = re.search(r"Data:\s*([\d\s\.\-eE+]+)", result)

if match:
    actual_data = match.group(1).strip()
    # Split by any whitespace and convert to floats
    actual = np.array([float(x) for x in actual_data.split()], dtype=np.float32)
    
    if np.allclose(actual, expected, atol=1e-4):
        print("SUCCESS: Compiler matches NumPy!")
    else:
        print(f"FAILURE: Expected {expected}, got {actual}")
else:
    print("ERROR: Regex could not find 'Data: ' in the output.")
    print("--- FULL DRIVER OUTPUT ---")
    print(result)
    print("---------------------------")