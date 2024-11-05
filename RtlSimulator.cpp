//
// Created by hanyuan on 2024/11/5.
//

#include "RtlSimulator.h"
#include <iostream>
#include <stdexcept>

void RtlSimulatorEndSymbol::propagate(std::size_t pos, const CircuitData &data) {
    inputDataVec[pos] = data;
    inputReadyVec[pos] = true;
    if (getReadyInputs() == static_cast<int>(inputDataVec.size())) {
        resetReadyInputs();
        auto outputData = calculateOutput();
        std::cout << identifier << " output: " << outputData.toString() << std::endl;
    }
}

RtlSimulatorEndSymbol::RtlSimulatorEndSymbol(std::shared_ptr<ModuleIOPort> port) : CircuitSymbolWire(
        port->getIdentifier()) {

}

RtlSimulator::RtlSimulator(const RtlModule &module) : rtlModule(module) {
    for (auto &port: module.ioPorts) {
        if (port->getPortDirection() == PortDirection::Input) {
            inputPorts.push_back(port);
        } else if (port->getPortDirection() == PortDirection::Output) {
            auto endSym = std::make_shared<RtlSimulatorEndSymbol>(port);
            endSym->registerInput(port);
            outputSymbols.push_back(endSym);
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
        throw std::runtime_error("No such an input port");
    }

    port->propagate(0, data);
}
