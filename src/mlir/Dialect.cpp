#include "dlc/Dialect.h"

#include "mlir/IR/Attributes.h"
#include "mlir/IR/Dialect.h"
#include "mlir/IR/Builders.h"
#include "mlir/IR/BuiltinTypes.h"
#include "mlir/IR/OpImplementation.h"
#include "mlir/IR/Operation.h"
#include "mlir/IR/Value.h"
#include "mlir/Interfaces/FunctionImplementation.h"
#include "mlir/Support/LLVM.h"
#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Support/Casting.h"
#include <algorithm>
#include <string>

using namespace mlir;
using namespace mlir::dlc;

#include "dlc/Dialect.cpp.inc"

//===----------------------------------------------------------------------===//
// Dlc Dialect
//===----------------------------------------------------------------------===//

// Dialect initialization, the instance will be owned by the context.
// This is the point of registration of types and operations for the dialect.
void mlir::dlc::DlcDialect::initialize() {
    addOperations<
#define GET_OP_LIST
#include "dlc/Ops.cpp.inc"
    >();
}

#define GET_OP_CLASSES
#include "dlc/Ops.cpp.inc"