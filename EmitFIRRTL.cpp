//
// Created by hanyuan on 2024/11/20.
//

#include "EmitFIRRTL.h"
#include "Parser.h"
#include <circt/Dialect/FIRRTL/FIRRTLAnnotations.h>
#include <circt/Dialect/FIRRTL/FIRRTLOps.h>
#include <circt/Dialect/FIRRTL/FIRRTLTypes.h>
#include <circt/Dialect/FIRRTL/FIRRTLUtils.h>
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
    /* 由 output 开始向后推 */
    circt::ArrayAttr layers;
    circt::SmallVector<circt::firrtl::PortInfo, 8> ports;
    context.loadDialect<circt::firrtl::FIRRTLDialect>();

    auto mlirModule = mlir::ModuleOp::create(mlir::UnknownLoc::get(&context));
    circt::OpBuilder b{mlirModule.getBodyRegion()};
    auto circuit = b.create<circt::firrtl::CircuitOp>(mlir::UnknownLoc::get(&context),
                                                      circt::StringAttr::get(&context,
                                                                             circt::StringRef{rtlModule.moduleName}));

    for (const auto &ioPort: rtlModule.ioPorts) {
        auto name = circt::StringAttr::get(&context, circt::StringRef{ioPort->getIdentifier()});
        auto type = circt::firrtl::UIntType::get(&context, ioPort->getSlicing().getWidth());
        auto direction = circt::firrtl::direction::get(ioPort->getPortDirection() == PortDirection::Output);
        ports.push_back({name, type, direction, {}});
    }

    auto builder = circuit.getBodyBuilder();
    auto moduleOp = builder.create<circt::firrtl::FModuleOp>(mlir::UnknownLoc::get(&context),
                                                             circt::StringAttr::get(&context, circt::StringRef{
                                                                     rtlModule.moduleName}),
                                                             circt::firrtl::ConventionAttr::get(&context,
                                                                                                circt::firrtl::Convention::Internal),
                                                             ports, emptyArrayAttr, layers
    );
    auto &body = moduleOp->getRegion(0).front();
    auto portArgs = body.getArguments();
    for (auto tuple: llvm::zip(ports, portArgs)) {
        circt::firrtl::PortInfo &port = std::get<0>(tuple);
        circt::BlockArgument portArg = std::get<1>(tuple);
        symbolTable[port.getName().str()] = portArg;
    }

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
