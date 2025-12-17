cmake_minimum_required(VERSION 3.20)
project(deep_learning_compiler LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17 CACHE STRING "C++ standard to conform to")

set(LLVM_DIR "/workspace/llvm-project/build/lib/cmake/llvm" CACHE PATH "Path to LLVMConfig.cmake")
set(MLIR_DIR "/workspace/llvm-project/build/lib/cmake/mlir" CACHE PATH "Path to MLIRConfig.cmake")
set(ONNX_INCLUDE_DIR "/workspace/onnx")

list(APPEND CMAKE_PREFIX_PATH "${LLVM_DIR}")
list(APPEND CMAKE_PREFIX_PATH "${MLIR_DIR}")

find_package(LLVM REQUIRED CONFIG)
find_package(MLIR REQUIRED CONFIG)
find_package(Protobuf REQUIRED)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION} in ${LLVM_DIR}")
message(STATUS "Found MLIR in ${MLIR_DIR}")

include_directories(${LLVM_INCLUDE_DIRS} ${MLIR_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

list(APPEND CMAKE_MODULE_PATH "${LLVM_CMAKE_DIR}")
list(APPEND CMAKE_MODULE_PATH "${MLIR_CMAKE_DIR}")

include(TableGen)
include(AddLLVM)
include(AddMLIR)

execute_process(
  COMMAND /workspace/llvm-project/build/bin/llvm-config --libs core support option remarks bitstreamreader binaryformat targetparser demangle
  OUTPUT_VARIABLE LLVM_LIBS
  OUTPUT_STRIP_TRAILING_WHITESPACE
)

string(REPLACE " " ";" LLVM_LIBS_LIST ${LLVM_LIBS})

link_directories("/workspace/llvm-project/build/lib")

add_executable(driver driver.cpp ${ONNX_INCLUDE_DIR}/onnx/onnx.pb.cc)

target_include_directories(driver PRIVATE
    ${CMAKE_BINARY_DIR}/include
    ${ONNX_INCLUDE_DIR}
    ${LLVM_INCLUDE_DIRS}
    ${MLIR_INCLUDE_DIRS}
)

target_link_libraries(driver
    PRIVATE
        LLVM-14
        LLVMOption
        MLIRSupport
        MLIRIR
        MLIRParser
        protobuf
)
message(STATUS "LLVM libs: ${LLVM_LIBS_LIST}")

set_target_properties(driver PROPERTIES
    BUILD_RPATH "/workspace/llvm-project/build/lib"
)
