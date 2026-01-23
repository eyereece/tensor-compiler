import onnxruntime as ort
import numpy as np

onnx_path = "onnx_files/test_2x1.onnx"

# Load model and input data
sess = ort.InferenceSession(onnx_path)
input_data = np.array([1.0,2.0], dtype=np.float32)

# Run model
result = sess.run(["final_out"], {"x": input_data})

print(f"Input: {input_data}")
print(f"Output: {result[0]}")