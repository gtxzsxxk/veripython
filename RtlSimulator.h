//
// Created by hanyuan on 2024/11/5.
//

#ifndef VERIPYTHON_RTLSIMULATOR_H
#define VERIPYTHON_RTLSIMULATOR_H

#include "RtlModule.h"
#include <memory>
#include <vector>

class RtlSimulatorEndSymbol : public CircuitSymbolWire {
public:
    RtlSimulatorEndSymbol(std::shared_ptr<ModuleIOPort> port);

    void propagate(std::size_t pos, const CircuitData &data) override;
};

class RtlSimulator {
    const RtlModule &rtlModule;
    std::vector<std::shared_ptr<ModuleIOPort>> inputPorts;
    std::vector<std::shared_ptr<RtlSimulatorEndSymbol>> outputSymbols;
public:
    explicit RtlSimulator(const RtlModule &module);

    [[nodiscard]] const decltype(inputPorts) &getInputPorts() const;

    void poke(std::string idName, const CircuitData &data);
};

#endif //VERIPYTHON_RTLSIMULATOR_H
