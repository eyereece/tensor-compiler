# Tensor Compiler

This is my implementation of a minimal Tensor compiler built with MLIR. I implemented a lowering pipeline starting from an ONNX graph down to LLVM IR, and executes via JIT compilation. The following is the highlight of the performance on a 2048 x 2048 matrix multiplication workload on an ARM CPU (M1 Pro 2021)

| Metric | Naïve / Baseline | Transposed | Transposed + Tiled |
| :--- | :---: | :---: | :---: |
| **Execution Time** | 56.0s | 9.7s | 5.8s |
| **L1 Data Misses** | 8.59B | 273M | 426M |
| **LLC Data Misses** | 8.59B | 273M | 9M |

### Lowering Pipeline
![lowering pipeline](./images/mlir-lowering.png)

The compiler follows a progressive lowering strategy to transform high-level ONNX graphs into LLVM IR. The current scope is focused on the core operations required to execute a Linear Layer (MatMul, add, ReLU, and Constant).

Compilation Flow:
ONNX -> DLC (Custom Dialect) -> Tensor/Linalg -> MemRef -> LLVM IR -> JIT Execution

Key Transformation:
- Frontend: Ingest ONNX models and lowers them into a custom dialect (named dlc)
- Mid-Level: Transitions to the Tensor/Linalg dialect to perform hardware-agnostic optimizations such as Transpose-B for contiguous memory access and Tile-and-Fuse to maximize cache residency
- Backend: Performs Bufferization to map tensors to physical memory (MemRef) before final lowering to the LLVM backend for JIT execution

### Optimization and Cache Analysis
The performance leap from 56s to 5.8s is primarily driven by optimizing cache locality.

<img src="./images/transposeb-bfore-after.png" alt="transpose RHS matrix before and after" width="80%"/>

#### Spatial Locality: Transpose-B
In a naïve MatMul, the Right-Hand Side (RHS) matrix is accessed by column. Since memory is row-major, this forces the CPU to fetch a new full cache line for every single floating-point value (evicting useful data before it’s ever used).
By transposing the RHS matrix during lowering to the linalg dialect, the algorithm’s access pattern is aligned with the hardware’s memory layout:
- As in in the “After” visual above, transposing turns vertical column jumps into horizontal row strides
- A single cache fetch now provides multiple subsequent values (highlighted in green), rather than just one
- This transformation reduced L1 Data Misses by 20x

<img src="./images/tiling.png" alt="tiling matrix" width="80%" />

#### Temporal Locality: Tiling
Loop tiling solves temporal locality (reusing data before it is evicted).
- By blocking the workload into smaller register-sized tiles, the working set fits entirely within the processor's caching hierarchy. This keeps the data alive locally for repetitive calculations
- This transformation reduces the Last-Level Cache Misses further by 30x

### Verification and Testing
Results are cross-referenced against ONNX Runtime with an absolute tolerance of 1e-3, confirming that the transformations maintain numerical integrity.
To ensure IR correctness and correct error handling, I implemented tests using LLVM Lit and Filecheck

### Codebase Directory
```
deep-learning-compiler
├── src/
│   ├── include/dlc                 # ops.td and header files
│   ├── mlir/                       # lowering passes
│   ├── models/                     # sample onnx models to run
│   ├── parser/                     # load onnx models into structured model info
│   ├── tests/                      # test files
│   ├── CMakeLists.txt
│   └── driver.cpp                  # main entry point compiler driver
├── .gitignore
├── LICENSE
├── Dockerfile
└── README.md
```

### How to run
There are multiple ways to run the project, I run this project inside a Docker container. If you already have llvm-project built, you can clone this project and move on to step #1. (make sure that this project is on the same level directory as the llvm-project)

To run with Docker, build with the Dockerfile provided and then clone the project inside the container. I've only built this project on a Macbook M1 Pro so far.

Step #1:
Run cmake in deep-learning-compiler/src:
```
mkdir -p build
cd build
cmake ..
make
```

To run (in the build folder):
```
./driver -emit=<FLAG> path/to/model
# for example
./driver -emit=proto ../models/onnx_files/add_constant.onnx

# available flags:
# proto, mlir, mlir-tensor, mlir-memref, mlir-llvm, llvm, jit
```

Use a script to run large matrices, the following python script is available to run MxM matrices:
go to src/tests/Correctness
```
# From the project root:
python3 src/tests/Correctness/verify_matmul.py
```

Testing (run from build dir, make sure to run from the correct dir):
```
../../../llvm-project/build/bin/llvm-lit -v ../tests/
```

### Implementation Deep Dive
I have written a series of articles on the implementation details of the project available here:
1. Building my first MLIR-based Tensor Compiler
2. Building a minimal MLIR Pipeline from ONNX to LLVM JIT
3. Implementing Linear Layers and Memory Optimizations on MLIR
4. Performance Analysis and Testing