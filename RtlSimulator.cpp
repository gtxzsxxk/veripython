//
// Created by hanyuan on 2024/11/5.
//

#include "RtlSimulator.h"
#include "Exceptions.h"
#include <iostream>

void RtlSimulatorEndSymbol::propagate(std::size_t pos, const CircuitData &data) {
    inputDataVec[pos] = std::make_pair(data, inputDataVec[pos].second);
    outputData = calculateOutput();
}

RtlSimulatorEndSymbol::RtlSimulatorEndSymbol(std::shared_ptr<ModuleIOPort> port) :
        CircuitSymbolWire(port->getIdentifier(), port->getSlicing()) {

}

RtlSimulator::RtlSimulator(const RtlModule &module) : rtlModule(module) {
    for (auto &port: module.ioPorts) {
        if (port->getPortDirection() == PortDirection::Input) {
            inputPorts.push_back(port);
        } else if (port->getPortDirection() == PortDirection::Output) {
            auto endSym = std::make_shared<RtlSimulatorEndSymbol>(port);
            endSym->registerInput(port, port->getSlicing());
            outputSymbols.push_back(endSym);
        }
    }
    for (auto &sym: module.circuitSymbols) {
        if (sym->getIdentifier().starts_with("__hwconst_")) {
            constantSymbols.push_back(sym);
        }
    }
}

const decltype(RtlSimulator::inputPorts) &RtlSimulator::getInputPorts() const {
    return inputPorts;
}

void RtlSimulator::poke(std::string idName, const CircuitData &data) {
    std::shared_ptr<ModuleIOPort> port = nullptr;
    for (auto &inPort: inputPorts) {
        if (inPort->getIdentifier() == idName) {
            port = inPort;
            break;
        }
    }

    if (port == nullptr) {
        throw CircuitException("No such an input port");
    }

    port->propagate(0, data);

    for (auto &constSym: constantSymbols) {
        constSym->propagate(0, CircuitData{PortSlicingAST{0, 0}});
    }

    for (auto &reg: rtlModule.registers) {
        reg->propagateStoredData();
    }
}

CircuitData RtlSimulator::peek(std::string idName) {
    for (auto &outPort: outputSymbols) {
        if (outPort->getIdentifier() == idName) {
            return outPort->getOutputData();
        }
    }

    throw CircuitException("No such an output port");
}

void RtlSimulator::printOutcome() {
    for (auto &outputSym: outputSymbols) {
        auto outputData = outputSym->getOutputData();
        std::cout << outputSym->getIdentifier() << " output: " << outputData.toString() << std::endl;
    }
}
