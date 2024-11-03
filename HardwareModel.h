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

class CircuitConnection {
    std::string destIdentifier;
    HDLExpressionAST *ast;
public:
    explicit CircuitConnection(std::string dest,
                               HDLExpressionAST *connAST) :
            destIdentifier(std::move(dest)),
            ast(connAST) {}
};

class CircuitSymbol {
protected:
    std::string identifier;
public:
    explicit CircuitSymbol(std::string identifier) : identifier(std::move(identifier)) {}
};

class ModuleIOPort : public CircuitSymbol {
    PortDirection direction = PortDirection::Unspecified;
    PortSlicingAST *slicing = nullptr;
public:
    explicit ModuleIOPort(std::string identifier) :
            CircuitSymbol(std::move(identifier)) {}

    explicit ModuleIOPort(PortDirection direction,
                          std::string identifier) :
            CircuitSymbol(std::move(identifier)),
            direction(direction) {}

    explicit ModuleIOPort(PortDirection direction, PortSlicingAST *slicing,
                          std::string identifier) :
            CircuitSymbol(std::move(identifier)),
            direction(direction),
            slicing(slicing) {}

    [[nodiscard]] const std::string &getPortName() const;

    void setPortDirection(PortDirection newDirection);
};

class CircuitSymbolWire : public CircuitSymbol {
public:
    explicit CircuitSymbolWire(std::string identifier) : CircuitSymbol(std::move(identifier)) {}
};

class CircuitSymbolReg : public CircuitSymbol {
public:
    explicit CircuitSymbolReg(std::string identifier) : CircuitSymbol(std::move(identifier)) {}
};

class HardwareModule {
    std::vector<CircuitConnection> circuitConnections;
public:
    std::vector<ModuleIOPort> ioPorts;
    std::vector<CircuitSymbol *> circuitSymbols;
    std::string moduleName;

    ModuleIOPort &getModuleIOPortByName(const std::string &name);

    void addCircuitConnection(CircuitConnection &&connection);


};


#endif //VERIPYTHON_HARDWAREMODEL_H
