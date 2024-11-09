//
// Created by hanyuan on 2024/11/2.
//

#include "RtlModule.h"
#include "CombLogics.h"
#include <stdexcept>

std::shared_ptr<ModuleIOPort> RtlModule::getModuleIOPortByName(const std::string &name) {
    for (auto port: ioPorts) {
        if (port->getIdentifier() == name) {
            return port;
        }
    }

    throw std::runtime_error("No such an I/O port");
}

void RtlModule::addCircuitConnection(CircuitConnection &&connection) {
    circuitConnections.push_back(std::move(connection));
}

std::shared_ptr<CircuitSymbol> RtlModule::getPortOrSymbolById(const std::string &id) {
    for (auto &ioPort: ioPorts) {
        if (ioPort->getIdentifier() == id) {
            return ioPort;
        }
    }

    for (auto symbol: circuitSymbols) {
        if (symbol->getIdentifier() == id) {
            return symbol;
        }
    }

    throw std::runtime_error("No such an identifier when lookup the circuit");
    return nullptr;
}

std::shared_ptr<CircuitSymbol> RtlModule::genCircuitSymbolByHDLExprAST(HDLExpressionAST *ast) {
    if (ast->nodeType == "const_number") {
        auto constSym = std::make_shared<CircuitSymbolConstant>(dynamic_cast<HDLPrimaryAST *>(ast));
        circuitSymbols.push_back(constSym);
        return constSym;
    } else if (ast->nodeType == "identifier") {
        return getPortOrSymbolById(dynamic_cast<HDLPrimaryAST *>(ast)->getIdentifier());
    } else {
        if (!HDLExpressionAST::canParseToCombLogics(ast->_operator)) {
            throw std::runtime_error("AST is not an operator");
        }
        auto combLogic = CombLogicFactory::create(ast->_operator);
        for (const auto &child: ast->children) {
            auto hdlExprAST = dynamic_cast<HDLExpressionAST *>(child.get());
            auto circuitSymbol = genCircuitSymbolByHDLExprAST(hdlExprAST);
            if (hdlExprAST->exprSlicing.isTrivial()) {
                combLogic->registerInput(circuitSymbol);
            } else {
                combLogic->registerInput(circuitSymbol, hdlExprAST->exprSlicing);
            }
        }
        std::shared_ptr<CombLogic> sharedCombLogic = std::move(combLogic);
        circuitSymbols.push_back(sharedCombLogic);
        return sharedCombLogic;
    }
}

void RtlModule::buildCircuit() {
    for (auto &conn: circuitConnections) {
        auto *ast = conn.getHDLExpressionAST();
        auto symbol = genCircuitSymbolByHDLExprAST(ast);
        int inputSlicingNextStart = 0;
        for (auto &[destIdentifier, destSlicing]: conn.getDestIdentifiers()) {
            auto destSymbol = getPortOrSymbolById(destIdentifier);
            auto destWidth = destSlicing.isDownTo ? destSlicing.downToHigh - destSlicing.downToLow + 1 : 1;
            if (destSlicing.isTrivial()) {
                destWidth = destSymbol->getSlicing().downToHigh - destSymbol->getSlicing().downToLow + 1;
            }
            PortSlicingAST inputSlicingInTotal = {destWidth - 1, 0};
            inputSlicingInTotal.downToHigh += inputSlicingNextStart;
            inputSlicingInTotal.downToLow += inputSlicingNextStart;
            inputSlicingNextStart += destWidth;

            if (destSlicing.isTrivial()) {
                destSymbol->registerInput(symbol, inputSlicingInTotal);
            } else {
                auto destWireSymbol = std::static_pointer_cast<CircuitSymbolWire>(destSymbol);
                if (destWireSymbol) {
                    destWireSymbol->registerInput(symbol, destSlicing, inputSlicingInTotal);
                } else {
                    throw std::runtime_error("Cannot set slicing");
                }
            }
        }
    }
}
