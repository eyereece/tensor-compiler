// OPS.TD dlc dialect operation definition (tablegen)
// defines the operations of the Dlc dialect

#ifndef DLC_OPS
#define DLC_OPS

include "mlir/IR/OpBase.td"
include "mlir/Interfaces/FunctionInterfaces.td"
include "mlir/IR/SymbolInterfaces.td"
include "mlir/Interfaces/SideEffectInterfaces.td"
include "mlir/Interfaces/InferTypeOpInterface.td"


// definition of the dlc dialect in the ODS framework
def Dlc_Dialect : Dialect {
    let name = "dlc";
    let summary = "A high-level dialect for analyzing and optimizing the onnx IR";
    let cppNamespace = "::mlir::dlc";
}

// Base class for Dlc dialect operations.
// This operation inherits from the base 'Op' class in OpBase.td.
class Dlc_Op<string mnemonic, list<Trait> traits = []> :
    Op<Dlc_Dialect, mnemonic, traits>;

//===----------------------------------------------------------------------===//
// Dlc Operations
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
// ConstantOp
//===----------------------------------------------------------------------===//
def ConstantOp : Dlc_Op<"constant", [Pure]> {
    let summary = "constant";
    let description = [{
        Constant operation turns a literal into an SSA value. The data is attached
        to the operation as an attribute. For example:

        ```mlir
            %0 = dlc.constant dense<[[1.0, 2.0, 3.0], [4.0, 5.0, 6.0]]>
                                : tensor<2x3xf32>
        ```
    }];

    let arguments = (ins ElementsAttr:$value);
    let results = (outs F32Tensor);
    let hasVerifier = 1;
    let builders = [
        OpBuilder<(ins "DenseElementsAttr":$value), [{
            build($_builder, $_state, value.getType(), value);
        }]>
    ];
}

//===----------------------------------------------------------------------===//
// AddOp
//===----------------------------------------------------------------------===//
def AddOp : Dlc_Op<"add"> {
    let summary = "addition operation";
    let arguments = (ins F32Tensor:$lhs, F32Tensor:$rhs);
    let results = (outs F32Tensor);
    let hasVerifier = 1;
    let builders = [
        OpBuilder<(ins "Value":$lhs, "Value":$rhs), [{
            build($_builder, $_state, lhs.getType(), lhs, rhs);
        }]>
    ];
}

//===----------------------------------------------------------------------===//
// ReluOp
//===----------------------------------------------------------------------===//
def ReluOp : Dlc_Op<"relu", [Pure, SameOperandsAndResultType]> {
    let summary = "rectified linear unit activation";
    let description = [{
        Element-wise Rectified Linear Unit operation.
        f(x) = max(0, x)
    }];
    let arguments = (ins F32Tensor:$input);
    let results = (outs F32Tensor:$output);
    let assemblyFormat = "$input attr-dict `:` type($input)";
}


#endif // DLC_OPS
