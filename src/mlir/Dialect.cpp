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
#include "mlir/IR/BuiltinTypes.h"
#include "mlir/Support/LogicalResult.h"
#include "mlir/Interfaces/InferTypeOpInterface.h"
#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Support/Casting.h"
#include <algorithm>
#include <string>

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

namespace mlir {
namespace dlc {

LogicalResult ConstantOp::verify() {
    // Get the attribute value and the result type
    auto tensorAttr = llvm::dyn_cast<DenseElementsAttr>(getValue());
    if (!tensorAttr) {
        return emitOpError("requires a dense elements attribute");
    }
    auto tensorType = llvm::cast<RankedTensorType>(getResult().getType());

    // Check if the num of elements matches
    if (tensorAttr.getNumElements() != tensorType.getNumElements()) {
        return emitOpError() << "number of elements in 'value' attribute ("
                                << tensorAttr.getNumElements()
                                << ") does not match the result type ("
                                << tensorType.getNumElements() << ")";
    }
    return success();
}

LogicalResult AddOp::verify() {
    // Get the types of the operands and the result
    auto lhsType = llvm::cast<RankedTensorType>(getLhs().getType());
    auto rhsType = llvm::cast<RankedTensorType>(getRhs().getType());
    auto resType = llvm::cast<RankedTensorType>(getResult().getType());

    // Check if LHS and RHS shapes match
    if (lhsType.getShape() != rhsType.getShape()) {
        return emitOpError() << "requires LHS and RHS to have the same shape, but got "
                                << lhsType << " and " << rhsType;
    }

    // Check if result shape matches input shape
    if (resType.getShape() != lhsType.getShape()) {
        return emitOpError() << "requires result shape to match input shape";
    }

    return success();
}

}   // namespace dlc
}   // namespace mlir