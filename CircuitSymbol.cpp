//
// Created by hanyuan on 2024/11/4.
//

#include "CircuitSymbol.h"
#include <iostream>

int CircuitSymbolConstant::counter = 0;

std::string CircuitConnection::getDestIdentifier() const {
    return destIdentifier;
}

HDLExpressionAST *CircuitConnection::getHDLExpressionAST() const {
    return ast.get();
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