cmake_minimum_required(VERSION 3.20)
project(deep_learning_compiler C CXX)

if(NOT LLVM_ENABLE_RTTI)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-rtti")
endif()

# --- 1. Hardcoded Paths ---
set(LLVM_DIR "/workspace/llvm-project/build/lib/cmake/llvm" CACHE PATH "Path to LLVMConfig.cmake")
set(MLIR_DIR "/workspace/llvm-project/build/lib/cmake/mlir" CACHE PATH "Path to MLIRConfig.cmake")
set(ONNX_INCLUDE_DIR "/workspace/onnx")

# --- 2. LLVM & MLIR Configuration ---
list(APPEND CMAKE_PREFIX_PATH "${LLVM_DIR}" "${MLIR_DIR}")

find_package(LLVM REQUIRED CONFIG)
find_package(MLIR REQUIRED CONFIG)
find_package(Protobuf REQUIRED)

list(APPEND CMAKE_MODULE_PATH "${LLVM_CMAKE_DIR}" "${MLIR_CMAKE_DIR}")

include(TableGen)
include(AddLLVM)
include(AddMLIR)

# Standard MLIR include directories
include_directories(${LLVM_INCLUDE_DIRS} ${MLIR_INCLUDE_DIRS} ${ONNX_INCLUDE_DIR})
add_definitions(${LLVM_DEFINITIONS})

# --- 3. TableGen (Dialect & Ops Generation) ---
set(LLVM_TARGET_DEFINITIONS ${CMAKE_CURRENT_SOURCE_DIR}/include/dlc/Ops.td)
set(DLC_GEN_DIR ${CMAKE_CURRENT_BINARY_DIR}/include/dlc)
file(MAKE_DIRECTORY ${DLC_GEN_DIR})

# Clear the list to ensure no stale entries
set(TABLEGEN_OUTPUT "")

# Generate Dialect Declarations
mlir_tablegen(include/dlc/Dialect.h.inc -gen-dialect-decls -dialect=dlc)
list(APPEND TABLEGEN_OUTPUT ${DLC_GEN_DIR}/Dialect.h.inc)

# Generate Dialect Definitions
mlir_tablegen(include/dlc/Dialect.cpp.inc -gen-dialect-defs -dialect=dlc)
list(APPEND TABLEGEN_OUTPUT ${DLC_GEN_DIR}/Dialect.cpp.inc)

# Generate Op Declarations
mlir_tablegen(include/dlc/Ops.h.inc -gen-op-decls)
list(APPEND TABLEGEN_OUTPUT ${DLC_GEN_DIR}/Ops.h.inc)

# Generate Op Definitions
mlir_tablegen(include/dlc/Ops.cpp.inc -gen-op-defs)
list(APPEND TABLEGEN_OUTPUT ${DLC_GEN_DIR}/Ops.cpp.inc)

add_public_tablegen_target(DLC_OPS_GEN)

# --- 4. Targets ---

# A. Parser Library
add_library(parser STATIC parser/Parser.cpp)
target_include_directories(parser PUBLIC include)
target_link_libraries(parser PUBLIC protobuf)

# B. Dialect & MLIR Logic Library
add_library(dlc_dialect STATIC
    mlir/Dialect.cpp
    mlir/MLIRGen.cpp
    mlir/LowerToTensor.cpp
    mlir/LowerToMemref.cpp
)
add_dependencies(dlc_dialect DLC_OPS_GEN)
target_include_directories(dlc_dialect PUBLIC include ${CMAKE_CURRENT_BINARY_DIR}/include)

# C. Main Driver
add_executable(driver 
    driver.cpp 
    ${ONNX_INCLUDE_DIR}/onnx/onnx.pb.cc
)

# Use target_link_libraries to handle dependencies and propagation
target_link_libraries(driver
    PRIVATE
        parser
        dlc_dialect
        # MLIR Libraries

        # Dialects
        MLIRPass
        MLIRTransforms
        MLIRLinalgDialect
        MLIRTensorDialect
        MLIRArithDialect
        MLIRMemRefDialect
        MLIRBufferizationDialect
        MLIRFuncDialect

        # Transformation & Bufferization Logic
        MLIRBufferizationTransforms
        MLIRLinalgTransforms
        MLIRTensorTransforms

        # MLIRGen
        MLIRIR
        MLIRParser
        MLIRBytecodeReader
        MLIRAsmParser
        MLIRSupport
        # MLIRControlFlowInterfaces
        # MLIRInferTypeOpInterface
        # MLIRSideEffectInterfaces
        # LLVM Libraries
        LLVMSupport
        LLVMCore
)

# --- 5. RPATH Configuration (So it finds LLVM .so files) ---
set_target_properties(driver PROPERTIES 
    BUILD_WITH_INSTALL_RPATH TRUE
    INSTALL_RPATH "/workspace/llvm-project/build/lib"
)