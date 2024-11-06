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

std::size_t CircuitSymbol::registerInput(std::shared_ptr<CircuitSymbol> symbol,
                                         const PortSlicingAST &inputSlicing) {
    int currentPos = static_cast<int>(inputDataVec.size());
    if (currentPos >= getMaxInputs()) {
        throw std::runtime_error("Cannot bind more input ports!");
    }
    inputDataVec.emplace_back(CircuitData{slicing}, slicing);
    inputReadyVec.push_back(false);
    std::pair<bool, PortSlicingAST> inputSlicingPair{false, {-1, -1}};
    if (!inputSlicing.isTrivial()) {
        inputSlicingPair = std::make_pair(true, inputSlicing);
    }
    symbol->propagateTargets.emplace_back(currentPos, this, inputSlicingPair);
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
        for (auto [nextPos, nextSymbol, propagateSlicing]: propagateTargets) {
            if (propagateSlicing.first) {
                auto propagateSlicingData = propagateSlicing.second;
                CircuitData slicedData{propagateSlicingData};
                if (propagateSlicingData.isDownTo) {
                    for (auto i = propagateSlicingData.downToLow;
                         i <= propagateSlicingData.downToHigh; i++) {
                        slicedData.bits[i - propagateSlicingData.downToLow] = data.bits[i];
                    }
                } else {
                    slicedData.bits[0] = data.bits[propagateSlicingData.onlyWhich];
                }
                nextSymbol->propagate(nextPos, slicedData);
            } else {
                nextSymbol->propagate(nextPos, outputData);
            }
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

std::size_t CircuitSymbolWire::registerInput(std::shared_ptr<CircuitSymbol> symbol,
                                             const PortSlicingAST &destSlicing,
                                             const PortSlicingAST &inputSlicing) {
    int currentPos = static_cast<int>(inputDataVec.size());
    for (auto [_, existSlicing]: inputDataVec) {
        for (int i = 0; i <= slicing.downToHigh; i++) {
            if (existSlicing.isDownTo) {
                if (existSlicing.isDownTo) {
                    if (i >= existSlicing.downToLow && i <= existSlicing.downToHigh &&
                        i >= destSlicing.downToLow && i <= destSlicing.downToHigh) {
                        throw std::runtime_error("Assignment overlapping");
                    }
                } else {
                    if (i >= existSlicing.downToLow && i <= existSlicing.downToHigh &&
                        i == destSlicing.onlyWhich) {
                        throw std::runtime_error("Assignment overlapping");
                    }
                }
            } else {
                if (destSlicing.isDownTo) {
                    if (i == existSlicing.onlyWhich &&
                        i >= destSlicing.downToLow && i <= destSlicing.downToHigh) {
                        throw std::runtime_error("Assignment overlapping");
                    }
                } else {
                    if (i == existSlicing.onlyWhich &&
                        i == destSlicing.onlyWhich) {
                        throw std::runtime_error("Assignment overlapping");
                    }
                }
            }
        }
    }
    inputDataVec.emplace_back(CircuitData{slicing}, destSlicing);
    inputReadyVec.push_back(false);
    std::pair<bool, PortSlicingAST> inputSlicingPair{false, {-1, -1}};
    if (!inputSlicing.isTrivial()) {
        inputSlicingPair = std::make_pair(true, inputSlicing);
    }
    symbol->propagateTargets.emplace_back(currentPos, this, inputSlicingPair);
    return currentPos;
}

void CircuitSymbolWire::propagate(std::size_t pos, const CircuitData &data) {
    auto destSlicing = inputDataVec[pos].second;
    auto inputData = inputDataVec[0].first;
    if (destSlicing.isDownTo) {
        for (auto i = destSlicing.downToLow;
             i <= destSlicing.downToHigh && i - destSlicing.downToLow <= data.slicing.downToHigh; i++) {
            inputData.bits[i] = data.bits[i - destSlicing.downToLow];
        }
    } else {
        inputData.bits[destSlicing.onlyWhich] = data.bits[0];
    }
    inputDataVec[0] = std::make_pair(inputData, destSlicing);
    outputData = calculateOutput();
    for (auto [nextPos, nextSymbol, propagateSlicing]: propagateTargets) {
        if (propagateSlicing.first) {
            auto propagateSlicingData = propagateSlicing.second;
            CircuitData slicedData{propagateSlicingData};
            if (propagateSlicingData.isDownTo) {
                for (auto i = propagateSlicingData.downToLow;
                     i <= propagateSlicingData.downToHigh; i++) {
                    slicedData.bits[i - propagateSlicingData.downToLow] = data.bits[i];
                }
            } else {
                slicedData.bits[0] = data.bits[propagateSlicingData.onlyWhich];
            }
            nextSymbol->propagate(nextPos, slicedData);
        } else {
            nextSymbol->propagate(nextPos, outputData);
        }
    }
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
