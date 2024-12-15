//
// Created by hanyuan on 2024/12/15.
//

#include "ArcBackend.h"
#include "Exceptions.h"
#include <circt/Dialect/Arc/ArcPasses.h>
#include <circt/Dialect/Arc/ArcOps.h>
#include <circt/Dialect/Arc/ArcInterfaces.h>
#include <circt/Dialect/HW/HWPasses.h>
#include <circt/Dialect/OM/OMDialect.h>
#include <circt/Dialect/SV/SVDialect.h>
#include <circt/Conversion/ConvertToArcs.h>
#include <circt/Conversion/CombToArith.h>
#include <circt/Conversion/ArcToLLVM.h>
#include <circt/Conversion/Passes.h>
#include <circt/Support/Passes.h>
#include <mlir/Dialect/Arith/IR/Arith.h>
#include <mlir/Dialect/ControlFlow/IR/ControlFlow.h>
#include <mlir/Dialect/DLTI/DLTI.h>
#include <mlir/Dialect/Func/Extensions/InlinerExtension.h>
#include <mlir/Dialect/Func/IR/FuncOps.h>
#include <mlir/Dialect/Index/IR/IndexDialect.h>
#include <mlir/Dialect/LLVMIR/LLVMDialect.h>
#include <mlir/Dialect/LLVMIR/Transforms/InlinerInterfaceImpl.h>
#include <mlir/Dialect/SCF/IR/SCF.h>
#include <mlir/Pass/PassManager.h>
#include <mlir/Target/LLVMIR/Dialect/All.h>
#include <mlir/Transforms/Passes.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>

/* Copied from circt/tools/arcilator */
void ArcBackend::convertHWToArc(mlir::ModuleOp &moduleOp) {
    circt::arc::registerPasses();
    /* lower to arc */
    mlir::PassManager pm(&context);

    pm.addPass(circt::createLowerFirMemPass());
    {
        circt::arc::AddTapsOptions opts;
        opts.tapPorts = false;
        opts.tapWires = false;
        opts.tapNamedValues = false;
        pm.addPass(circt::arc::createAddTapsPass(opts));
    }
    pm.addPass(circt::arc::createStripSVPass());
    {
        circt::arc::InferMemoriesOptions opts;
        opts.tapPorts = false;
        opts.tapMemories = false;
        pm.addPass(circt::arc::createInferMemoriesPass(opts));
    }
    pm.addPass(mlir::createCSEPass());
    pm.addPass(circt::arc::createArcCanonicalizerPass());
    {
        circt::ConvertToArcsOptions opts;
        opts.tapRegisters = false;
        pm.addPass(circt::createConvertToArcsPass(opts));
    }
    pm.addPass(circt::arc::createDedupPass());
    pm.addPass(circt::hw::createFlattenModulesPass());
    pm.addPass(mlir::createCSEPass());
    pm.addPass(circt::arc::createArcCanonicalizerPass());

    pm.addPass(circt::arc::createSplitLoopsPass());
    pm.addPass(circt::arc::createDedupPass());
    {
        circt::arc::InferStatePropertiesOptions opts;
        opts.detectEnables = true;
        opts.detectResets = false;
        pm.addPass(circt::arc::createInferStateProperties(opts));
    }
    pm.addPass(mlir::createCSEPass());
    pm.addPass(circt::arc::createArcCanonicalizerPass());
    pm.addPass(circt::arc::createMakeTablesPass());
    pm.addPass(mlir::createCSEPass());
    pm.addPass(circt::arc::createArcCanonicalizerPass());

    // Now some arguments may be unused because reset conditions are not passed as
    // inputs anymore pm.addPass(arc::createRemoveUnusedArcArgumentsPass());
    // Because we replace a lot of StateOp inputs with constants in the enable
    // patterns we may be able to sink a lot of them
    // TODO: maybe merge RemoveUnusedArcArguments with SinkInputs?
    // pm.addPass(arc::createSinkInputsPass());
    // pm.addPass(createCSEPass());
    // pm.addPass(createSimpleCanonicalizerPass());
    // Removing some muxes etc. may lead to additional dedup opportunities
    // if (shouldDedup)
    // pm.addPass(arc::createDedupPass());

    // Lower stateful arcs into explicit state reads and writes.
    pm.addPass(circt::arc::createLowerStatePass());

    // TODO: LowerClocksToFuncsPass might not properly consider scf.if operations
    // (or nested regions in general) and thus errors out when muxes are also
    // converted in the hw.module or arc.model
    // TODO: InlineArcs seems to not properly handle scf.if operations, thus the
    // following is commented out
    // pm.addPass(arc::createMuxToControlFlowPass());
    pm.addPass(circt::arc::createInlineArcsPass());

    pm.addPass(circt::arc::createMergeIfsPass());
    pm.addPass(mlir::createCSEPass());
    pm.addPass(circt::arc::createArcCanonicalizerPass());

    // Allocate states.
    pm.addPass(circt::arc::createLowerArcsToFuncsPass());
    pm.nest<circt::arc::ModelOp>().addPass(circt::arc::createAllocateStatePass());
    pm.addPass(circt::arc::createLowerClocksToFuncsPass()); // no CSE between state alloc
    // and clock func lowering
    pm.addPass(mlir::createCSEPass());
    pm.addPass(circt::arc::createArcCanonicalizerPass());

    if (mlir::failed(pm.run(moduleOp))) {
        throw CircuitException("Failed to run hw to arc passes");
    }
}

std::string ArcBackend::convertArcToLLVMIR(mlir::ModuleOp &moduleOp) {
    mlir::PassManager pm(&context);
    pm.addPass(circt::createConvertCombToArithPass());
    pm.addPass(circt::createLowerArcToLLVMPass());
    pm.addPass(mlir::createCSEPass());
    pm.addPass(circt::arc::createArcCanonicalizerPass());

    if (mlir::failed(pm.run(moduleOp))) {
        throw CircuitException("Failed to run arc to llvm passes");
    }

    llvm::LLVMContext llvmContext;
    auto llvmModule = mlir::translateModuleToLLVMIR(moduleOp, llvmContext);
    if (!llvmModule) {
        throw CircuitException("Failed to translate MLIR module to LLVM IR");
    }

    std::string irStr;
    llvm::raw_string_ostream outs{irStr};
    llvmModule->print(outs, nullptr);
    return irStr;
}

void ArcBackend::arcExportState() {

}

ArcBackend::ArcBackend(mlir::MLIRContext &context) : context(context) {
    mlir::DialectRegistry registry;
    registry.insert<
            circt::arc::ArcDialect,
            circt::comb::CombDialect,
            circt::hw::HWDialect,
            mlir::arith::ArithDialect,
            mlir::cf::ControlFlowDialect,
            mlir::DLTIDialect,
            mlir::func::FuncDialect,
            mlir::index::IndexDialect,
            mlir::LLVM::LLVMDialect,
            mlir::scf::SCFDialect,
            circt::om::OMDialect,
            circt::seq::SeqDialect,
            circt::sv::SVDialect
    >();

    circt::arc::initAllExternalInterfaces(registry);

    mlir::func::registerInlinerExtension(registry);
    mlir::LLVM::registerInlinerInterface(registry);

    mlir::registerBuiltinDialectTranslation(registry);
    mlir::registerLLVMDialectTranslation(registry);
    context.appendDialectRegistry(registry);
}
