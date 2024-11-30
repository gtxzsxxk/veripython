//
// Created by hanyuan on 2024/11/20.
//

#include "EmitFIRRTL.h"
#include "Parser.h"
#include <circt/Dialect/FIRRTL/FIRRTLOps.h>
#include <circt/Dialect/FIRRTL/FIRRTLTypes.h>
#include <mlir/IR/MLIRContext.h>
#include <mlir/IR/ImplicitLocOpBuilder.h>

circt::Value EmitFIRRTL::emitFromSymbol(std::shared_ptr<CircuitSymbol> &symbol) {
    if (symbol->getIdentifier().starts_with("__comb_" + Parser::operatorName[TOKEN_question])) {

    }
    return {};
}

void EmitFIRRTL::emit() {
    auto hwConnections = rtlModule.getHardwareConnections();
    for (const auto &conn: hwConnections) {

    }
}

bool EmitFIRRTL::isOutputPort(std::string &identifier) const {
    for (const auto &port: rtlModule.ioPorts) {
        if (port->getPortDirection() == PortDirection::Output && port->getIdentifier() == identifier) {
            return true;
        }
    }
    return false;
}
