//
// Created by hanyuan on 2024/11/4.
//

#ifndef VERIPYTHON_CIRCUITSYMBOL_H
#define VERIPYTHON_CIRCUITSYMBOL_H

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

class CircuitInnerData {
public:
    std::vector<bool> bits;
    PortSlicingAST slicing{0, 0};

    explicit CircuitInnerData(const PortSlicingAST &slicingData);

    [[nodiscard]] std::size_t getBitWidth() const;

    [[nodiscard]] std::string toString() const;
};

class CircuitSimOutputData : public CircuitInnerData {
public:
    std::vector<char> bits;

    explicit CircuitSimOutputData(const PortSlicingAST &slicingData);

    explicit CircuitSimOutputData(const CircuitInnerData &circuitInnerData);

    [[nodiscard]] std::string toString() const;
};

class CircuitSymbol {
protected:
    std::string identifier;
    PortSlicingAST slicing;
    std::vector<CircuitInnerData> inputDataVec;
    std::vector<bool> inputReadyVec;
    std::vector<std::pair<std::size_t, CircuitSymbol *>> propagateTargets;
    CircuitInnerData outputData{PortSlicingAST{0, 0}};
    bool outputDataValid = false;

    virtual CircuitInnerData calculateOutput() = 0;

    virtual int getMaxInputs() = 0;

    [[nodiscard]] int getReadyInputs() const;

    void resetReadyInputs();

public:
    explicit CircuitSymbol(std::string identifier) :
            identifier(std::move(identifier)),
            slicing(PortSlicingAST(0, 0)) {}

    [[nodiscard]] const PortSlicingAST &getSlicing() const;

    std::size_t registerInput(std::shared_ptr<CircuitSymbol> symbol);

    virtual void propagate(std::size_t pos, const CircuitInnerData &data);

    CircuitSimOutputData getOutputData();

    [[nodiscard]] std::string getIdentifier() const;

    [[nodiscard]] const decltype(propagateTargets) &getPropagateTargets() const;

    virtual ~CircuitSymbol() = default;
};

class CircuitSymbolWire : public CircuitSymbol {
protected:
    CircuitInnerData calculateOutput() override;

    int getMaxInputs() override;

public:
    explicit CircuitSymbolWire(std::string identifier,
                               const PortSlicingAST &slicingAst) :
            CircuitSymbol(std::move(identifier)) {
        slicing = slicingAst;
    }
};

class CircuitSymbolConstant : public CircuitSymbolWire {
    static int counter;
    int value;
    int width;
protected:

    CircuitInnerData calculateOutput() override;

    int getMaxInputs() override;

public:
    explicit CircuitSymbolConstant(HDLPrimaryAST *ast) :
            CircuitSymbolWire("__hwconst_" + std::to_string(counter++), PortSlicingAST{ast->width - 1, 0}),
            value(ast->value), width(ast->width) {
        if (ast->isIdentifier()) {
            throw std::runtime_error("The ast can't be an identifier");
        }
        if (width == 0) {
            throw std::runtime_error("Bad width");
        } else if (width > 1) {
            slicing = PortSlicingAST{width - 1, 0};
        }
        inputDataVec.emplace_back(slicing);
        inputReadyVec.push_back(false);
    }
};

class ModuleIOPort : public CircuitSymbolWire {
    PortDirection direction = PortDirection::Unspecified;

    void registerForInput();

public:
    explicit ModuleIOPort(std::string identifier) :
            CircuitSymbolWire(std::move(identifier), {0, 0}) {}

    explicit ModuleIOPort(PortDirection direction,
                          std::string identifier) :
            CircuitSymbolWire(std::move(identifier), {0, 0}),
            direction(direction) {
        if (direction == PortDirection::Input) {
            inputDataVec.emplace_back(slicing);
            inputReadyVec.push_back(false);
        }
    }

    explicit ModuleIOPort(PortDirection direction, const PortSlicingAST &slicingAST,
                          std::string identifier) :
            CircuitSymbolWire(std::move(identifier), {0, 0}),
            direction(direction) {
        slicing = slicingAST;
        if (direction == PortDirection::Input) {
            inputDataVec.emplace_back(slicing);
            inputReadyVec.push_back(false);
        }
    }

    PortDirection getPortDirection() const;

    void setPortDirection(PortDirection newDirection);
};

//class CircuitSymbolReg : public CircuitSymbol {
//public:
//    explicit CircuitSymbolReg(std::string identifier) : CircuitSymbol(std::move(identifier)) {}
//};

#endif //VERIPYTHON_CIRCUITSYMBOL_H
