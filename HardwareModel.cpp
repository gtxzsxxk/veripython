//
// Created by hanyuan on 2024/11/2.
//

#include "HardwareModel.h"
#include "CombLogics.h"
#include <iostream>
#include <stdexcept>

int CircuitSymbolConstant::counter = 0;

std::string CircuitConnection::getDestIdentifier() const {
    return destIdentifier;
}

HDLExpressionAST *CircuitConnection::getHDLExpressionAST() const {
    return ast;
}

CircuitData::CircuitData(const PortSlicingAST &slicingData) {
    if (slicingData.isDownTo) {
        for (int i = slicingData.downToLow; i <= slicingData.downToHigh; i++) {
            bits.push_back(false);
        }
    } else {
        bits.push_back(false);
    }
}

std::size_t CircuitData::getBitWidth() const {
    return bits.size();
}

const PortSlicingAST &CircuitSymbol::getSlicing() const {
    return slicing;
}

std::size_t CircuitSymbol::registerInput(std::shared_ptr<CircuitSymbol> symbol) {
    int currentPos = static_cast<int>(inputDataVec.size());
    if (currentPos >= getMaxInputs()) {
        throw std::runtime_error("Cannot bind more input ports!");
    }
    inputDataVec.emplace_back(symbol->slicing);
    symbol->propagateTargets.emplace_back(currentPos, this);
    return currentPos;
}

void CircuitSymbol::propagate(std::size_t pos, const CircuitData &data) {
    inputDataVec[pos] = data;
    /* TODO: 检查这个计数方法正确不正确 */
    readyInputs++;
    if (readyInputs == static_cast<int>(inputDataVec.size())) {
        readyInputs = 0;
        auto outputData = calculateOutput();
        for (auto [nextPos, nextSymbol]: propagateTargets) {
            nextSymbol->propagate(nextPos, outputData);
        }
    }
}

std::string CircuitSymbol::getIdentifier() const {
    return identifier;
}

CircuitData CircuitSymbolConstant::calculateOutput() {
    CircuitData data{slicing};
    int tmpValue = value;
    for (int i = 0; i < width; i++) {
        data.bits[i] = (tmpValue & 0x01UL) == 1;
        tmpValue >>= 1;
    }
    return data;
}

int CircuitSymbolConstant::getMaxInputs() {
    return 0;
}

CircuitData CircuitSymbolWire::calculateOutput() {
    return inputDataVec[0];
}

int CircuitSymbolWire::getMaxInputs() {
    return 1;
}

const std::string &ModuleIOPort::getPortName() const {
    return identifier;
}

void ModuleIOPort::setPortDirection(PortDirection newDirection) {
    if (direction != PortDirection::Unspecified) {
        throw std::runtime_error("This port's direction has been specified");
    }
    direction = newDirection;
}

void ModuleIOPort::propagate(std::size_t pos, const CircuitData &data) {
    inputDataVec[pos] = data;
    /* TODO: 检查这个计数方法正确不正确 */
    readyInputs++;
    if (readyInputs == static_cast<int>(inputDataVec.size())) {
        readyInputs = 0;
        auto outputData = calculateOutput();
        std::cout << identifier << " compute finished" << std::endl;
    }
}

std::shared_ptr<ModuleIOPort> HardwareModule::getModuleIOPortByName(const std::string &name) {
    for (auto port: ioPorts) {
        if (port->getPortName() == name) {
            return port;
        }
    }

    throw std::runtime_error("No such an I/O port");
}

void HardwareModule::addCircuitConnection(CircuitConnection &&connection) {
    circuitConnections.push_back(connection);
}

std::shared_ptr<CircuitSymbol> HardwareModule::getPortOrSymbolById(const std::string &id) {
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

std::shared_ptr<CircuitSymbol> HardwareModule::genCircuitSymbolByHDLExprAST(HDLExpressionAST *ast) {
    if (ast->nodeType == "const_number") {
        return std::make_shared<CircuitSymbolConstant>(dynamic_cast<HDLPrimaryAST *>(ast));
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

void HardwareModule::buildCircuit() {
    for (auto &conn: circuitConnections) {
        auto destSymbol = getPortOrSymbolById(conn.getDestIdentifier());
        auto *ast = conn.getHDLExpressionAST();
        auto symbol = genCircuitSymbolByHDLExprAST(ast);
        destSymbol->registerInput(symbol);
    }
}
