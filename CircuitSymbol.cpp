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

std::string CircuitData::toString() const {
    std::string ret;
    for (auto i = (long int) (bits.size() - 1); i >= 0; i--) {
        ret += (bits[i] ? "1" : "0");
        ret += " ";
    }
    return ret;
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
    inputReadyVec.push_back(false);
    symbol->propagateTargets.emplace_back(currentPos, this);
    return currentPos;
}

const decltype(CircuitSymbol::propagateTargets) &CircuitSymbol::getPropagateTargets() const {
    return propagateTargets;
}

void CircuitSymbol::propagate(std::size_t pos, const CircuitData &data) {
    inputDataVec[pos] = data;
    inputReadyVec[pos] = true;
    if (getReadyInputs() == static_cast<int>(inputDataVec.size())) {
        resetReadyInputs();
        auto outputData = calculateOutput();
        for (auto [nextPos, nextSymbol]: propagateTargets) {
            nextSymbol->propagate(nextPos, outputData);
        }
    }
}

std::string CircuitSymbol::getIdentifier() const {
    return identifier;
}

int CircuitSymbol::getReadyInputs() const {
    int counter = 0;
    for (const auto ready: inputReadyVec) {
        if (ready) {
            counter++;
        }
    }
    return counter;
}

void CircuitSymbol::resetReadyInputs() {
    for (std::size_t i = 0; i < inputReadyVec.size(); i++) {
        inputReadyVec[i] = false;
    }
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

void ModuleIOPort::registerForInput() {
    if (direction == PortDirection::Input) {
        inputDataVec.clear();
        inputDataVec.emplace_back(slicing);
    }
}

void ModuleIOPort::setPortDirection(PortDirection newDirection) {
    if (direction != PortDirection::Unspecified) {
        throw std::runtime_error("This port's direction has been specified");
    }
    direction = newDirection;
    registerForInput();
}

PortDirection ModuleIOPort::getPortDirection() const {
    return direction;
}
