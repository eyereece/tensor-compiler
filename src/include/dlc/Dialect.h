#ifndef DLC_DIALECT_H
#define DLC_DIALECT_H

#include "mlir/Bytecode/BytecodeOpInterface.h"
#include "mlir/IR/Dialect.h"
#include "mlir/IR/SymbolTable.h"
#include "mlir/Interfaces/CallInterfaces.h"
#include "mlir/Interfaces/FunctionInterfaces.h"
#include "mlir/Interfaces/SideEffectInterfaces.h"
#include "mlir/Interfaces/InferTypeOpInterface.h"

// Include auto-generated header file containing the declaration of the dlc dialect
#include "dlc/Dialect.h.inc"

// Include auto-generated header file containing the declarations of the dlc operations
#define GET_OP_CLASSES
#include "dlc/Ops.h.inc"

#endif  // DLC_DIALECT_H