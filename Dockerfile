# Use official Ubuntu 22.04 image
FROM ubuntu:22.04

# Prevent interactive prompts during package installation
ENV DEBIAN_FRONTEND=noninteractive

# Install dependencies needed for llvm-project build
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    ninja-build \
    git \
    python3 \
    python3-pip \
    wget \
    curl \
    ca-certificates \
    libtinfo-dev \
    zlib1g-dev \
    libncurses5 \
    libncurses5-dev \
    && rm -rf /var/lib/apt/lists/*

# Set working directory inside container
WORKDIR /workspace

# Clone llvm-project
RUN git clone https://github.com/llvm/llvm-project.git

# Build LLVM/MLIR
RUN mkdir -p llvm-project/build && cd llvm-project/build && \
    cmake -G Ninja ../llvm -DLLVM_ENABLE_PROJECTS="mlir" \
    -DLLVM_BUILD_EXAMPLES=OFF -DLLVM_TARGETS_TO_BUILD="Native" \
    -DCMAKE_BUILD_TYPE=Release -DLLVM_ENABLE_ASSERTIONS=ON && \
    ninja -j4 && cmake --build . --target check-mlir

# Create dlc directory (source code goes here)
RUN mkdir -p /workspace/dlc-project

# Set working directory to dlc-project
WORKDIR /workspace/dlc-project

# Keep the container running with bash
CMD ["/bin/bash"]