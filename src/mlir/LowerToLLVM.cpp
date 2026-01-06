#include "dlc/Passes.h"

#include "mlir/Conversion/ArithToLLVM/ArithToLLVM.h"
#include "mlir/Conversion/ControlFlowToLLVM/ControlFlowToLLVM.h"
#include "mlir/Conversion/FuncToLLVM/ConvertFuncToLLVM.h"
#include "mlir/Conversion/FuncToLLVM/ConvertFuncToLLVMPass.h"
#include "mlir/Conversion/LLVMCommon/ConversionTarget.h"
#include "mlir/Conversion/LLVMCommon/TypeConverter.h"
#include "mlir/Conversion/MemRefToLLVM/MemRefToLLVM.h"
#include "mlir/Conversion/SCFToControlFlow/SCFToControlFlow.h"
#include "mlir/Conversion/ReconcileUnrealizedCasts/ReconcileUnrealizedCasts.h"
#include "mlir/Dialect/LLVMIR/LLVMDialect.h"
#include "mlir/Dialect/SCF/IR/SCF.h"
#include "mlir/Dialect/ControlFlow/IR/ControlFlow.h"
#include "mlir/Pass/Pass.h"
#include "mlir/Transforms/DialectConversion.h"

using namespace mlir;

namespace {
struct LowerToLLVMPass
    : public PassWrapper<LowerToLLVMPass, OperationPass<ModuleOp>> {
    MLIR_DEFINE_EXPLICIT_INTERNAL_INLINE_TYPE_ID(LowerToLLVMPass)

    void getDependentDialects(DialectRegistry &registry) const override {
        registry.insert<LLVM::LLVMDialect, scf::SCFDialect>();
    }

    void runOnOperation() final {
        // Define the target (what is legal)
        LLVMConversionTarget target(getContext());
        target.addLegalOp<mlir::ModuleOp>();

        // Define the Type Converter (How do types change)
        LLVMTypeConverter typeConverter(&getContext());

        // Collect all the patterns
        RewritePatternSet patterns(&getContext());

        // Patterns for the dialects used
        populateSCFToControlFlowConversionPatterns(patterns);
        arith::populateArithToLLVMConversionPatterns(typeConverter, patterns);
        populateFinalizeMemRefToLLVMConversionPatterns(typeConverter, patterns);
        cf::populateControlFlowToLLVMConversionPatterns(typeConverter, patterns);
        populateFuncToLLVMConversionPatterns(typeConverter, patterns);

        // Apply the conversion to the module
        auto module = getOperation();
        if (failed(applyFullConversion(module, target, std::move(patterns)))) {
            signalPassFailure();
        }
    }
};
}   // namespace

std::unique_ptr<mlir::Pass> dlc::createLowerToLLVMPass() {
    return std::make_unique<LowerToLLVMPass>();
}