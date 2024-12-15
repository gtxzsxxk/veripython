//
// Created by hanyuan on 2024/11/20.
//

#ifndef VERIPYTHON_EMITFIRRTL_H
#define VERIPYTHON_EMITFIRRTL_H

#include "RtlModule.h"
#include <unordered_map>
#include <circt/Support/LLVM.h>
#include <mlir/IR/Value.h>
#include <mlir/IR/Builders.h>
#include <mlir/IR/BuiltinOps.h>
#include <mlir/IR/ImplicitLocOpBuilder.h>

class EmitFIRRTL {
    const RtlModule &rtlModule;

    std::unordered_map<std::string, circt::Value> symbolTable{};

    circt::MLIRContext context{};

    const circt::ArrayAttr emptyArrayAttr{mlir::ArrayAttr::get(&context, {})};

    circt::Value combToCirctOpValue(const std::shared_ptr<CircuitSymbol> &symbol);

    circt::Value concatFromRange(const std::vector<circt::Value> &values, int start, int end);

    [[nodiscard]] bool isOutputPort(std::string &identifier) const;

public:
    explicit EmitFIRRTL(const RtlModule &module);

    circt::ImplicitLocOpBuilder implicitLocOpBuilder{mlir::UnknownLoc::get(&context), &context};

    mlir::ModuleOp emitModuleOp();

    static std::string ModuleToMLIR(mlir::ModuleOp &module);

    circt::Value
    emitFromSymbol(const std::shared_ptr<CircuitSymbol> &symbol, const PortSlicingAST &inputSlicing = {-1, -1});

    circt::MLIRContext &getContext();
};


#endif //VERIPYTHON_EMITFIRRTL_H
