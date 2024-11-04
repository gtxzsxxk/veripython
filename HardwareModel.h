//
// Created by hanyuan on 2024/11/2.
//

#ifndef VERIPYTHON_HARDWAREMODEL_H
#define VERIPYTHON_HARDWAREMODEL_H

#include "AST.h"
#include <memory>
#include <stdexcept>
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
    std::unique_ptr<HDLExpressionAST> ast;
public:
    CircuitConnection(std::string dest,
                      std::unique_ptr<HDLExpressionAST> connAST) :
            destIdentifier(std::move(dest)),
            ast(std::move(connAST)) {}

    CircuitConnection(CircuitConnection &&conn) {
        destIdentifier = conn.destIdentifier;
        ast = std::move(conn.ast);
    }

    [[nodiscard]] std::string getDestIdentifier() const;

    [[nodiscard]] HDLExpressionAST *getHDLExpressionAST() const;
};

class CircuitData {
public:
    std::vector<bool> bits;

    explicit CircuitData(const PortSlicingAST &slicingData);

    [[nodiscard]] std::size_t getBitWidth() const;
};

class CircuitSymbol {
protected:
    std::string identifier;
    PortSlicingAST slicing;
    int readyInputs = 0;
    std::vector<CircuitData> inputDataVec;
    std::vector<std::pair<std::size_t, std::shared_ptr<CircuitSymbol>>> propagateTargets;

    virtual CircuitData calculateOutput() = 0;

    virtual int getMaxInputs() = 0;

public:
    explicit CircuitSymbol(std::string identifier) :
            identifier(std::move(identifier)),
            slicing(PortSlicingAST(1)) {}

    [[nodiscard]] const PortSlicingAST &getSlicing() const;

    std::size_t registerInput(std::shared_ptr<CircuitSymbol> symbol);

    virtual void propagate(std::size_t pos, const CircuitData &data);

    [[nodiscard]] std::string getIdentifier() const;

    virtual ~CircuitSymbol() = default;
};

class CircuitSymbolConstant : public CircuitSymbol {
    static int counter;
    int value;
    int width;
protected:

    CircuitData calculateOutput() override;

    int getMaxInputs() override;

public:
    explicit CircuitSymbolConstant(HDLPrimaryAST *ast) : CircuitSymbol("__hwconst_" + std::to_string(counter++)),
                                                         value(ast->value), width(ast->width) {
        if (ast->isIdentifier()) {
            throw std::runtime_error("The ast can't be an identifier");
        }
        if (width == 0) {
            throw std::runtime_error("Bad width");
        } else if (width > 1) {
            slicing = PortSlicingAST{width - 1, 0};
        }
    }
};

class CircuitSymbolWire : public CircuitSymbol {
protected:
    CircuitData calculateOutput() override;

    int getMaxInputs() override;

public:
    explicit CircuitSymbolWire(std::string identifier) : CircuitSymbol(std::move(identifier)) {}
};

class ModuleIOPort : public CircuitSymbolWire {
    PortDirection direction = PortDirection::Unspecified;

public:
    explicit ModuleIOPort(std::string identifier) :
            CircuitSymbolWire(std::move(identifier)) {}

    explicit ModuleIOPort(PortDirection direction,
                          std::string identifier) :
            CircuitSymbolWire(std::move(identifier)),
            direction(direction) {}

    explicit ModuleIOPort(PortDirection direction, const PortSlicingAST &slicingAST,
                          std::string identifier) :
            CircuitSymbolWire(std::move(identifier)),
            direction(direction) {
        slicing = slicingAST;
    }

    [[nodiscard]] const std::string &getPortName() const;

    void setPortDirection(PortDirection newDirection);

    void propagate(std::size_t pos, const CircuitData &data) override;
};

//class CircuitSymbolReg : public CircuitSymbol {
//public:
//    explicit CircuitSymbolReg(std::string identifier) : CircuitSymbol(std::move(identifier)) {}
//};

class HardwareModule {
    std::vector<CircuitConnection> circuitConnections;

    std::shared_ptr<CircuitSymbol> getPortOrSymbolById(const std::string &id);

    std::shared_ptr<CircuitSymbol> genCircuitSymbolByHDLExprAST(HDLExpressionAST *ast);

public:
    std::vector<std::shared_ptr<ModuleIOPort>> ioPorts;
    std::vector<std::shared_ptr<CircuitSymbol>> circuitSymbols;
    std::string moduleName;

    std::shared_ptr<ModuleIOPort> getModuleIOPortByName(const std::string &name);

    void addCircuitConnection(CircuitConnection &&connection);

    void buildCircuit();
};


#endif //VERIPYTHON_HARDWAREMODEL_H
