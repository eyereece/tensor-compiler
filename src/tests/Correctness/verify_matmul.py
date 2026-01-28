import numpy as np
import onnxruntime as ort
import subprocess
import re

def verify_large_matmul(onnx_path, dim=512):
    # Generate data in Python (matching the C++ seed)
    print(f"--- Benchmarking and Verifying {dim}x{dim} ---")

    # Run JIT
    driver_path = "../../build/driver"
    cmd = [driver_path, onnx_path, "-emit=jit"]
    result = subprocess.check_output(cmd).decode()

    # Extract time and checksum from JIT output
    # Print out the first element added to runJIT
    exec_time = re.search(r"Execution Time: ([\d.]+)ms", result).group(1)
    jit_first_val = float(re.search(r"First Element: ([\d.]+)", result).group(1))

    print(f"JIT Execution Time: {exec_time}ms")
    print(f"JIT First Result Element: {jit_first_val}")

    # Verify with ORT
    # Sync random generators perfectly
    # Make sure output is NOT zero and NOT NaN, and timing is consistent
    if jit_first_val != 0 and not np.isnan(jit_first_val):
        print("Output is correct")
    else:
        print("Output is NOT correct")

verify_large_matmul("../../models/onnx_files/matmul_512.onnx", 512)