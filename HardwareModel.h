//
// Created by hanyuan on 2024/11/2.
//

#ifndef VERIPYTHON_HARDWAREMODEL_H
#define VERIPYTHON_HARDWAREMODEL_H

#include "AST.h"
#include <string>
#include <utility>
#include <vector>

enum class PortDirection {
    Input,
    Output,
    Unspecified
};

class ModuleIOPort {
    PortDirection direction = PortDirection::Unspecified;
    std::string identifier;
    PortSlicingAST *slicing = nullptr;
public:
    explicit ModuleIOPort(std::string identifier) :
            identifier(std::move(identifier)) {}

    explicit ModuleIOPort(PortDirection direction,
                          std::string identifier) : direction(direction),
                                                    identifier(std::move(identifier)) {}

    explicit ModuleIOPort(PortDirection direction, PortSlicingAST *slicing,
                          std::string identifier) : direction(direction),
                                                    identifier(std::move(identifier)),
                                                    slicing(slicing) {}
};

class HardwareModule {
public:
    std::vector<ModuleIOPort> ioPorts;
    std::string moduleName;
};


#endif //VERIPYTHON_HARDWAREMODEL_H
