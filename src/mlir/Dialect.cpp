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

//===----------------------------------------------------------------------===//
// ConstantOp
//===----------------------------------------------------------------------===//

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

//===----------------------------------------------------------------------===//
// AddOp
//===----------------------------------------------------------------------===//

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

//===----------------------------------------------------------------------===//
// MatmulOp
//===----------------------------------------------------------------------===//

void MatMulOp::build(OpBuilder &builder, OperationState &state,
                    Value lhs, Value rhs) {
    auto lhsType = llvm::cast<RankedTensorType>(lhs.getType());
    auto rhsType = llvm::cast<RankedTensorType>(rhs.getType());

    // MatMul Shape Logic: (M x K) * (K x N) -> (M x N)
    auto lhsShape = lhsType.getShape();
    auto rhsShape = rhsType.getShape();

    // Determine M (Rows): If 1D, treat it as 1 row
    int64_t m = (lhsShape.size() == 1) ? 1 : lhsShape[0];

    // Determine N (Cols): If 2D, take index 1. If 1D, treat it as 1 column
    int64_t n = (rhsShape.size() == 1) ? 1 : rhsShape[1];

    // Result is always treated as 2D [M, N]
    auto resultType = RankedTensorType::get({m, n}, lhsType.getElementType());

    // Push the calculated result type and the inputs into the state
    state.addTypes(resultType);
    state.addOperands({lhs, rhs});
}

LogicalResult MatMulOp::verify() {
    auto lhsShape = llvm::cast<RankedTensorType>(getLhs().getType()).getShape();
    auto rhsShape = llvm::cast<RankedTensorType>(getRhs().getType()).getShape();
    auto resShape = llvm::cast<RankedTensorType>(getResult().getType()).getShape();

    // Input Rank check
    if (lhsShape.size() < 1 || lhsShape.size() > 2 ||
        rhsShape.size() < 1 || rhsShape.size() > 2) {
        return emitOpError("LHS and RHS must be either 1D or 2D tensors");
    }

    // Inner Dimension (K) check
    int64_t lhsK = lhsShape.back();
    int64_t rhsK = rhsShape[0];
    if (lhsK != rhsK) {
        return emitOpError("inner dimensions (K) must match: ")
                << lhsK << " (LHS) != " << rhsK << " (RHS)";
    }

    // Result Shape Verification
    // The expected rows (M)
    int64_t expectedM = (lhsShape.size() == 1) ? 1 : lhsShape[0];
    // The expected columns (N)
    int64_t expectedN = (rhsShape.size() == 1) ? 1 : rhsShape[1];

    // Result Check
    if (resShape.size() == 1) {
        if (resShape[0] != (expectedM * expectedN)) {
            return emitOpError("1D result size mismatch");
        }
    } else if (resShape.size() == 2) {
        if (resShape[0] != expectedM || resShape[1] != expectedN) {
            return emitOpError("2D result shape mismatch");
        }
    } else {
        return emitOpError("result must be 1D or 2D");
    }
    return success();
}
}   // namespace dlc
}   // namespace mlir