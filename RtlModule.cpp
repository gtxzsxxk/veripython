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
        if (!ast->isOperator()) {
            throw std::runtime_error("AST is not an operator");
        }
        auto combLogic = CombLogicFactory::create(ast->_operator);
        for (const auto &child: ast->children) {
            auto circuitSymbol = genCircuitSymbolByHDLExprAST(dynamic_cast<HDLExpressionAST *>(child.get()));
            combLogic->registerInput(circuitSymbol);
        }
        std::shared_ptr<CombLogic> sharedCombLogic = std::move(combLogic);
        circuitSymbols.push_back(sharedCombLogic);
        return sharedCombLogic;
    }
}

void RtlModule::buildCircuit() {
    for (auto &conn: circuitConnections) {
        auto destSymbol = getPortOrSymbolById(conn.getDestIdentifier());
        auto *ast = conn.getHDLExpressionAST();
        auto symbol = genCircuitSymbolByHDLExprAST(ast);
        if (conn.isDestSlicingTrivial()) {
            destSymbol->registerInput(symbol);
        } else {
            destSymbol->registerInput(symbol, conn.getDestSlicing());
        }
    }
}
