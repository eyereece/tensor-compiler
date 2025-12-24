cmake_minimum_required(VERSION 3.20)
project(deep_learning_compiler C CXX)

if(LLVM_ENABLE_RTTI)
  # LLVM was built with RTTI, do nothing
else()
  if(MSVC)
    add_definitions(/GR-)
  else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-rtti")
  endif()
endif()

set(CMAKE_CXX_STANDARD 17 CACHE STRING "C++ standard to conform to")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${LLVM_DEFINITIONS}")
set(LLVM_DIR "/workspace/llvm-project/build/lib/cmake/llvm" CACHE PATH "Path to LLVMConfig.cmake")
set(MLIR_DIR "/workspace/llvm-project/build/lib/cmake/mlir" CACHE PATH "Path to MLIRConfig.cmake")
set(ONNX_INCLUDE_DIR "/workspace/onnx")
set(LLVM_TARGET_DEFINITIONS ${CMAKE_CURRENT_SOURCE_DIR}/include/dlc/Ops.td)

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

link_directories("/workspace/llvm-project/build/lib")

# Create the directory in the build folder so mlir-tblgen can write into it
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/include/dlc)

# Generate Ops classes
# Generate the Dialect Header (dlc/Dialect.h.inc)
mlir_tablegen(include/dlc/Dialect.h.inc -gen-dialect-decls -dialect=dlc)
add_public_tablegen_target(DLC_DIALECT_HEADER_GEN)

mlir_tablegen(include/dlc/Dialect.cpp.inc -gen-dialect-defs -dialect=dlc)
add_public_tablegen_target(DLC_DIALECT_SRC_GEN)

# Generate the Ops Header (dlc/Ops.h.inc)
mlir_tablegen(include/dlc/Ops.h.inc -gen-op-decls)
add_public_tablegen_target(DLC_OPS_HEADER_GEN)

# Generate the Ops Source (dlc/Ops.cpp.inc)
mlir_tablegen(include/dlc/Ops.cpp.inc -gen-op-defs)
add_public_tablegen_target(DLC_OPS_SRC_GEN)

# add_library(dlc INTERFACE)

# create a library for the dialect implementation (add the cpp files)
add_library(dlc_dialect
    mlir/Dialect.cpp
    mlir/MLIRGen.cpp
)

# target_include_directories(dlc INTERFACE
#     ${CMAKE_CURRENT_SOURCE_DIR}/include
# )

target_include_directories(dlc_dialect PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_BINARY_DIR}/include
    ${ONNX_INCLUDE_DIR}
    ${LLVM_INCLUDE_DIRS}
    ${MLIR_INCLUDE_DIRS}
)

# target_sources(dlc_dialect
#     PRIVATE
#     ${DLC_OPS_HEADER_GEN}
#     ${DLC_OPS_CPP_INC}
#     ${DLC_DIALECT_HEADER_GEN}
# )

add_dependencies(dlc_dialect
  DLC_OPS_HEADER_GEN
  DLC_OPS_SRC_GEN
  DLC_DIALECT_HEADER_GEN
  DLC_DIALECT_SRC_GEN
)

add_library(parser STATIC parser/Parser.cpp)

target_include_directories(parser PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${ONNX_INCLUDE_DIR}
    ${LLVM_INCLUDE_DIRS}
    ${MLIR_INCLUDE_DIRS}
)

target_link_libraries(parser
    PUBLIC
        protobuf
)

# Link MLIR and LLVM dependencies
target_link_libraries(dlc_dialect
    PRIVATE
    MLIRIR
    MLIRSupport
    MLIRFuncDialect
    LLVMCore
)

add_executable(driver driver.cpp ${ONNX_INCLUDE_DIR}/onnx/onnx.pb.cc)

target_link_libraries(dlc_dialect
    PRIVATE
        MLIRIR
        MLIRSupport
        LLVMCore
)

target_include_directories(driver PRIVATE
    ${CMAKE_BINARY_DIR}/include
    ${ONNX_INCLUDE_DIR}
    ${LLVM_INCLUDE_DIRS}
    ${MLIR_INCLUDE_DIRS}
)

link_directories(/workspace/llvm-project/build/lib)

target_link_libraries(driver
    PRIVATE
        parser
        dlc_dialect
        MLIRIR
        MLIRParser
        MLIRBytecodeReader
        MLIRAsmParser
        MLIRSupport
        MLIRControlFlowInterfaces
        MLIRInferTypeOpInterface
        MLIRSideEffectInterfaces
        LLVMSupport
        LLVMCore
        LLVMBinaryFormat
        LLVMRemarks
        LLVMBitstreamReader
        protobuf
)
message(STATUS "LLVM libs: ${LLVM_LIBS_LIST}")

# add generated files as sources
# target_sources(dlc_dialect
#     PRIVATE
#     ${DLC_OPS_HEADER_GEN}
#     ${DLC_OPS_SRC_GEN}
#     ${DLC_DIALECT_HEADER_GEN}
#     mlir/Dialect.cpp
#     mlir/MLIRGen.cpp
# )

# set_target_properties(driver PROPERTIES
#     BUILD_RPATH "/workspace/llvm-project/build/lib"
# )

set_target_properties(driver PROPERTIES 
    BUILD_WITH_INSTALL_RPATH TRUE
    INSTALL_RPATH "/workspace/llvm-project/build/lib"
)