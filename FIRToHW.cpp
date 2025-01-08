//
// Created by unknown on 2024/12/14.
//

#include "FIRToHW.h"
#include "Exceptions.h"
#include <circt/Firtool/Firtool.h>
#include <circt/Conversion/Passes.h>
#include <circt/Dialect/FIRRTL/FIRRTLDialect.h>
#include <circt/Dialect/HW/HWDialect.h>
#include <mlir/Pass/PassManager.h>

void FIRToHW::convertToHW(mlir::ModuleOp &moduleOp) {
    circt::registerLowerFIRRTLToHW();
    context.loadDialect<circt::hw::HWDialect>();

    mlir::PassManager pm(&context);
    circt::firtool::FirtoolOptions opt{};
    if (mlir::failed(circt::firtool::populateLowFIRRTLToHW(pm, opt))) {
        throw CircuitException("Failed to run low firrtl to hw");
    }
    if (mlir::failed(pm.run(moduleOp))) {
        throw CircuitException("Failed to run pass manager");
    }
}
