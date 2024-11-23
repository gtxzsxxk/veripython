//
// Created by hanyuan on 2024/11/20.
//

#ifndef VERIPYTHON_EMITFIRRTL_H
#define VERIPYTHON_EMITFIRRTL_H

#include "RtlModule.h"
#include <circt/Support/LLVM.h>
#include <mlir/IR/Value.h>
#include <mlir/IR/Builders.h>

class EmitFIRRTL {
    const RtlModule &rtlModule;

    circt::MLIRContext context{};

    circt::OpBuilder builder{&context};

    circt::Value emitFromSymbol(std::shared_ptr<CircuitSymbol> &symbol);

public:
    explicit EmitFIRRTL(const RtlModule &module) : rtlModule(module) {}

    void emit();
};


#endif //VERIPYTHON_EMITFIRRTL_H
