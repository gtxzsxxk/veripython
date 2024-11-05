//
// Created by hanyuan on 2024/11/4.
//

#include "RtlVisualizer.h"
#include "CircuitSymbol.h"
#include <cstdlib>
#include <memory>
#include <stdexcept>
#include <string>
#include <fstream>
#include <vector>

void RtlVisualizer::visualize(const RtlModule &hardwareModel, const std::string &filename) {
    std::ofstream outputStream(filename + ".txt");
    if (!outputStream) {
        throw std::runtime_error("Unable to open the file");
    }

    outputStream << "digraph RtlView {\n"
                 << "  fontname=\"Helvetica,Arial,sans-serif\"\n"
                 << "  node [fontname=\"Helvetica,Arial,sans-serif\"]\n"
                 << "  edge [fontname=\"Helvetica,Arial,sans-serif\"]\n";

    std::vector<std::shared_ptr<CircuitSymbol>> symbols;
    for (auto &symbol: hardwareModel.ioPorts) {
        symbols.push_back(symbol);
    }
    for (auto &symbol: hardwareModel.circuitSymbols) {
        symbols.push_back(symbol);
        if (symbol->getIdentifier().starts_with("__comb_")) {
            outputStream << "  " << symbol->getIdentifier() << " [shape=invtriangle, label=\""
                         << symbol->getIdentifier().substr(7) << "\"]\n";
        }
    }

    for (const auto &circuitSymbol: symbols) {
        for (const auto &[which, nextTarget]: circuitSymbol->getPropagateTargets()) {
            outputStream << "  " << circuitSymbol->getIdentifier() << " -> " << nextTarget->getIdentifier()
                         << std::endl;
        }
    }

    outputStream << "}" << std::endl;
    outputStream.close();

    system(("dot -Tpng " + filename + ".txt -o " + filename + ".png").c_str());
    std::remove((filename + ".txt").c_str());
}
