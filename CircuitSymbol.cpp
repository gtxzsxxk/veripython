//
// Created by hanyuan on 2024/11/4.
//

#include "CircuitSymbol.h"
#include <iostream>
#include <utility>

int CircuitSymbolConstant::counter = 0;

bool CircuitConnection::isDestSlicingTrivial() const {
    return destSlicingTrivial;
}

std::string CircuitConnection::getDestIdentifier() const {
    return destIdentifier;
}

HDLExpressionAST *CircuitConnection::getHDLExpressionAST() const {
    return ast.get();
}

PortSlicingAST CircuitConnection::getDestSlicing() const {
    return destSlicing;
}

CircuitData::CircuitData(const PortSlicingAST &slicingData) {
    slicing = slicingData;
    if (slicingData.isDownTo) {
        for (int i = slicingData.downToLow; i <= slicingData.downToHigh; i++) {
            bits.push_back(-1);
        }
    } else {
        bits.push_back(-1);
    }
}

std::size_t CircuitData::getBitWidth() const {
    return bits.size();
}

std::string CircuitData::toString() const {
    std::string ret;
    for (auto i = (long int) (bits.size() - 1); i >= 0; i--) {
        ret += ((bits[i] == 1) ? "1" : (bits[i] == 0 ? "0" : "X"));
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
    inputDataVec.emplace_back(CircuitData{symbol->slicing}, slicing);
    inputReadyVec.push_back(false);
    symbol->propagateTargets.emplace_back(currentPos, this);
    return currentPos;
}

std::size_t CircuitSymbol::registerInput(std::shared_ptr<CircuitSymbol> symbol, const PortSlicingAST &destSlicing) {
    int currentPos = static_cast<int>(inputDataVec.size());
    if (currentPos >= getMaxInputs()) {
        throw std::runtime_error("Cannot bind more input ports!");
    }
    inputDataVec.emplace_back(CircuitData{symbol->slicing}, destSlicing);
    inputReadyVec.push_back(false);
    symbol->propagateTargets.emplace_back(currentPos, this);
    return currentPos;
}

const decltype(CircuitSymbol::propagateTargets) &CircuitSymbol::getPropagateTargets() const {
    return propagateTargets;
}

void CircuitSymbol::propagate(std::size_t pos, const CircuitData &data) {
    inputDataVec[pos] = std::make_pair(data, inputDataVec[pos].second);
    inputReadyVec[pos] = true;
    if (getReadyInputs() == static_cast<int>(inputDataVec.size())) {
        resetReadyInputs();
        outputData = calculateOutput();
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

CircuitData CircuitSymbol::getOutputData() {
    return outputData;
}

CircuitData CircuitSymbol::getInputCircuitData(std::size_t which) {
    return inputDataVec[which].first;
}

CircuitData CircuitSymbolConstant::calculateOutput() {
    CircuitData data{slicing};
    int tmpValue = value;
    for (int i = 0; i < width; i++) {
        data.bits[i] = (char) (tmpValue & 0x01UL);
        tmpValue >>= 1;
    }
    return data;
}

int CircuitSymbolConstant::getMaxInputs() {
    return 0;
}

CircuitData CircuitSymbolWire::calculateOutput() {
    return getInputCircuitData(0);
}

int CircuitSymbolWire::getMaxInputs() {
    return 1;
}

void ModuleIOPort::registerForInput() {
    if (direction == PortDirection::Input) {
        inputDataVec.clear();
        inputDataVec.emplace_back(CircuitData{slicing}, slicing);
        inputReadyVec.push_back(false);
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
