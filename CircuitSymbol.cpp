//
// Created by unknown on 2024/11/4.
//

#include "CircuitSymbol.h"
#include <iostream>
#include <utility>

int CircuitSymbolConstant::counter = 0;

std::vector<std::pair<std::string, PortSlicingAST>> &CircuitConnection::getDestIdentifiers() {
    return destIdentifiers;
}

HDLExpressionAST *CircuitConnection::getHDLExpressionAST() const {
    return ast.get();
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

std::size_t CircuitSymbol::registerInput(const std::shared_ptr<CircuitSymbol> &symbol,
                                         const PortSlicingAST &inputSlicing) {
    int currentPos = static_cast<int>(inputDataVec.size());
    if (currentPos >= getMaxInputs()) {
        throw CircuitException("Cannot bind more input ports!");
    }
    inputDataVec.emplace_back(CircuitData{slicing}, slicing);
    backwardSymbols.emplace_back(symbol, inputSlicing, PortSlicingAST{-1, -1});
    symbol->propagateTargets.emplace_back(currentPos, this, inputSlicing);
    return currentPos;
}

const decltype(CircuitSymbol::propagateTargets) &CircuitSymbol::getPropagateTargets() const {
    return propagateTargets;
}

void CircuitSymbol::propagate(std::size_t pos, const CircuitData &data) {
    inputDataVec[pos] = std::make_pair(data, inputDataVec[pos].second);
    outputData = calculateOutput();
    for (auto &[nextPos, nextSymbol, propagateSlicing]: propagateTargets) {
        if (!propagateSlicing.isTrivial()) {
            CircuitData slicedData{propagateSlicing};
            if (propagateSlicing.isDownTo) {
                for (auto i = propagateSlicing.downToLow;
                     i <= propagateSlicing.downToHigh && i < (int) outputData.bits.size();
                     i++) {
                    slicedData.bits[i - propagateSlicing.downToLow] = outputData.bits[i];
                }
            } else {
                slicedData.bits[0] = outputData.bits[propagateSlicing.onlyWhich];
            }
            nextSymbol->propagate(nextPos, slicedData);
        } else {
            nextSymbol->propagate(nextPos, outputData);
        }
    }
}

std::string CircuitSymbol::getIdentifier() const {
    return identifier;
}

CircuitData CircuitSymbol::getOutputData() {
    return outputData;
}

CircuitData CircuitSymbol::getInputCircuitData(std::size_t which) {
    return inputDataVec[which].first;
}

const decltype(CircuitSymbol::backwardSymbols) &CircuitSymbol::getBackwardSymbols() const {
    return backwardSymbols;
}

bool CircuitSymbol::isRegisterSymbol() const {
    return isRegister;
}

bool CircuitSymbol::isWireSymbol() const {
    return isWire;
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

std::size_t CircuitSymbolWire::registerInput(const std::shared_ptr<CircuitSymbol> &symbol,
                                             const PortSlicingAST &destSlicing,
                                             const PortSlicingAST &inputSlicing) {
    int currentPos = static_cast<int>(inputDataVec.size());
    for (auto [_, existSlicing]: inputDataVec) {
        for (int i = 0; i <= slicing.downToHigh; i++) {
            if (existSlicing.isDownTo) {
                if (existSlicing.isDownTo) {
                    if (i >= existSlicing.downToLow && i <= existSlicing.downToHigh &&
                        i >= destSlicing.downToLow && i <= destSlicing.downToHigh) {
                        throw CircuitException("Assignment overlapping");
                    }
                } else {
                    if (i >= existSlicing.downToLow && i <= existSlicing.downToHigh &&
                        i == destSlicing.onlyWhich) {
                        throw CircuitException("Assignment overlapping");
                    }
                }
            } else {
                if (destSlicing.isDownTo) {
                    if (i == existSlicing.onlyWhich &&
                        i >= destSlicing.downToLow && i <= destSlicing.downToHigh) {
                        throw CircuitException("Assignment overlapping");
                    }
                } else {
                    if (i == existSlicing.onlyWhich &&
                        i == destSlicing.onlyWhich) {
                        throw CircuitException("Assignment overlapping");
                    }
                }
            }
        }
    }
    inputDataVec.emplace_back(CircuitData{slicing}, destSlicing);
    backwardSymbols.emplace_back(symbol, inputSlicing, destSlicing);
    symbol->propagateTargets.emplace_back(currentPos, this, inputSlicing);
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
    for (auto &[nextPos, nextSymbol, propagateSlicing]: propagateTargets) {
        if (!propagateSlicing.isTrivial()) {
            CircuitData slicedData{propagateSlicing};
            if (propagateSlicing.isDownTo) {
                for (auto i = propagateSlicing.downToLow;
                     i <= propagateSlicing.downToHigh;
                     i++) {
                    slicedData.bits[i - propagateSlicing.downToLow] = outputData.bits[i];
                }
            } else {
                slicedData.bits[0] = outputData.bits[propagateSlicing.onlyWhich];
            }
            nextSymbol->propagate(nextPos, slicedData);
        } else {
            nextSymbol->propagate(nextPos, outputData);
        }
    }
}

CircuitData CircuitSymbolReg::calculateOutput() {
    return storedData;
}

int CircuitSymbolReg::getMaxInputs() {
    return 2;
}

std::size_t CircuitSymbolReg::registerClock(const std::shared_ptr<CircuitSymbol> &symbol) {
    int currentPos = static_cast<int>(inputDataVec.size());
    if (currentPos >= getMaxInputs()) {
        throw CircuitException("Cannot bind more input ports!");
    }
    inputDataVec.emplace_back(CircuitData{clockSlicing}, clockSlicing);
    backwardSymbols.emplace_back(symbol, clockSlicing, PortSlicingAST{-1, -1});
    symbol->propagateTargets.emplace_back(currentPos, this, clockSlicing);
    clockSymbolName = symbol->getIdentifier();
    return currentPos;
}

void CircuitSymbolReg::propagate(std::size_t pos, const CircuitData &data) {
    if (pos == 0) {
        if (triggerType == TriggerEdgeType::POSITIVE_EDGE) {
            if (prevClockSignal.bits[0] == false && data.bits[0] == true) {
                storedData = getInputCircuitData(1);
            }
        } else if (triggerType == TriggerEdgeType::NEGATIVE_EDGE) {
            if (prevClockSignal.bits[0] == true && data.bits[0] == false) {
                storedData = getInputCircuitData(1);
            }
        } else {
            throw CircuitException("The trigger type is not specified");
        }

        prevClockSignal.bits[0] = data.bits[0];
    } else {
        auto destSlicing = inputDataVec[pos].second;
        auto inputData = inputDataVec[1].first;
        if (destSlicing.isDownTo) {
            for (auto i = destSlicing.downToLow;
                 i <= destSlicing.downToHigh && i - destSlicing.downToLow <= data.slicing.downToHigh; i++) {
                inputData.bits[i] = data.bits[i - destSlicing.downToLow];
            }
        } else {
            inputData.bits[destSlicing.onlyWhich] = data.bits[0];
        }
        inputDataVec[1] = std::make_pair(inputData, destSlicing);
    }
}

void CircuitSymbolReg::propagateStoredData() {
    outputData = calculateOutput();
    for (auto &[nextPos, nextSymbol, propagateSlicing]: propagateTargets) {
        if (!propagateSlicing.isTrivial()) {
            CircuitData slicedData{propagateSlicing};
            if (propagateSlicing.isDownTo) {
                for (auto i = propagateSlicing.downToLow;
                     i <= propagateSlicing.downToHigh;
                     i++) {
                    slicedData.bits[i - propagateSlicing.downToLow] = outputData.bits[i];
                }
            } else {
                slicedData.bits[0] = outputData.bits[propagateSlicing.onlyWhich];
            }
            nextSymbol->propagate(nextPos, slicedData);
        } else {
            nextSymbol->propagate(nextPos, outputData);
        }
    }
}

void CircuitSymbolReg::setTriggerType(TriggerEdgeType type) {
    triggerType = type;
}

void ModuleIOPort::registerForInput() {
    if (direction == PortDirection::Input) {
        inputDataVec.clear();
        inputDataVec.emplace_back(CircuitData{slicing}, slicing);
    }
}

void ModuleIOPort::setPortDirection(PortDirection newDirection) {
    if (direction != PortDirection::Unspecified) {
        throw CircuitException("This port's direction has been specified");
    }
    direction = newDirection;
    registerForInput();
}

PortDirection ModuleIOPort::getPortDirection() const {
    return direction;
}
