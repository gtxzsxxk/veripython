//
// Created by hanyuan on 2024/11/2.
//

#include "HardwareModel.h"
#include <iostream>
#include <stdexcept>

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

const PortSlicingAST *CircuitSymbol::getSlicing() const {
    return slicing;
}

CircuitSymbol::~CircuitSymbol() {
    /* TODO: delete slicing */
}

std::size_t CircuitSymbol::registerInput(CircuitSymbol *symbol) {
    auto currentPos = propagateTargets.size();
    if (currentPos >= getMaxInputs()) {
        throw std::runtime_error("Cannot bind more input ports!");
    }
    symbol->propagateTargets.emplace_back(currentPos, this);
    return currentPos;
}

void CircuitSymbol::propagate(std::size_t pos, const CircuitData &data) {
    inputDataVec[pos] = data;
    /* TODO: 检查这个计数方法正确不正确 */
    readyInputs++;
    if (readyInputs == inputDataVec.size()) {
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
    if (readyInputs == inputDataVec.size()) {
        readyInputs = 0;
        auto outputData = calculateOutput();
        std::cout << identifier << " compute finished" << std::endl;
    }
}

ModuleIOPort &HardwareModule::getModuleIOPortByName(const std::string &name) {
    for (auto &port: ioPorts) {
        if (port.getPortName() == name) {
            return port;
        }
    }

    throw std::runtime_error("No such an I/O port");
}

void HardwareModule::addCircuitConnection(CircuitConnection &&connection) {
    circuitConnections.push_back(connection);
}

CircuitSymbol *HardwareModule::getPortOrSymbolById(const std::string& id) {
    for(auto & ioPort : ioPorts) {
        if(ioPort.getIdentifier() == id) {
            return &ioPort;
        }
    }

    for(auto *symbol : circuitSymbols) {
        if(symbol->getIdentifier() == id) {
            return symbol;
        }
    }

    throw std::runtime_error("No such an identifier when lookup the circuit");
    return nullptr;
}

void HardwareModule::traverseHDLExprAST(HDLExpressionAST *ast) {

}

void HardwareModule::buildCircuit() {
    for(auto &conn : circuitConnections) {
        auto *destSymbol = getPortOrSymbolById(conn.getDestIdentifier());
        auto *ast = conn.getHDLExpressionAST();

    }
}
