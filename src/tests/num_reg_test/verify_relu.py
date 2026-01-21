# RUN: python3 %s

import numpy as np
import subprocess
import re
import os

# 1. Generate random data (including negative numbers to test ReLU properly)
# Generate values between -1.0 and 1.0
input_data = (np.random.rand(4) * 2 - 1).astype(np.float32)
expected = np.maximum(0, input_data)

# 2. Setup paths
script_dir = os.path.dirname(__file__)
driver_path = os.path.abspath(os.path.join(script_dir, "../../build/driver"))
temp_mlir_path = os.path.join(script_dir, "verify_relu.mlir")

# 3. Generate verify_relu.mlir
# Use .tolist() to format the numpy array for MLIR
mlir_template = f"""
module {{
  func.func @main() -> tensor<4xf32> attributes {{llvm.emit_c_interface}} {{
    %0 = arith.constant dense<{input_data.tolist()}> : tensor<4xf32>
    %1 = "dlc.relu"(%0) : (tensor<4xf32>) -> tensor<4xf32>
    return %1 : tensor<4xf32>
  }}
}}
"""
with open(temp_mlir_path, "w") as f: 
    f.write(mlir_template)

# 4. Execute JIT
try:
    result = subprocess.check_output([driver_path, "-emit=jit", temp_mlir_path]).decode()

    # 5. Parse result using Regex
    match = re.search(r"Data:\s*([\d\s\.\-eE+]+)", result)

    if match:
        actual_data = match.group(1).strip()
        actual = np.array([float(x) for x in actual_data.split()], dtype=np.float32)
        
        if np.allclose(actual, expected, atol=1e-5):
            print(f"SUCCESS: ReLU matches NumPy!")
            print(f"Input:    {input_data}")
            print(f"Output:   {actual}")
        else:
            print(f"FAILURE: Expected {expected}, got {actual}")
    else:
        print("ERROR: Regex could not find 'Data: ' in the output.")
        print(result)

finally:
    # Cleanup temp file
    if os.path.exists(temp_mlir_path):
        os.remove(temp_mlir_path)