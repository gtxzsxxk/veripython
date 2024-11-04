//
// Created by hanyuan on 2024/11/4.
//

#include "RtlVisualizer.h"
#include "CircuitSymbol.h"
#include <memory>
#include <string>
#include <vector>

void RtlVisualizer::visualize(const RtlModule &hardwareModel) {
    std::vector<std::shared_ptr<CircuitSymbol>> symbols;
    for (auto &symbol: hardwareModel.ioPorts) {

    }
}
