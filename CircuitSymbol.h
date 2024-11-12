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
    std::vector<std::pair<std::string, PortSlicingAST>> destIdentifiers;
public:
    std::unique_ptr<HDLExpressionAST> ast;
    std::unique_ptr<HDLExpressionAST> conditionAST = nullptr; /* for build always block */

    CircuitConnection(std::string dest,
                      std::unique_ptr<HDLExpressionAST> connAST) :
            ast(std::move(connAST)) {
        destIdentifiers.emplace_back(dest, PortSlicingAST{-1, -1});
    }

    CircuitConnection(std::string dest, const PortSlicingAST &destSlicing,
                      std::unique_ptr<HDLExpressionAST> connAST) :
            ast(std::move(connAST)) {
        destIdentifiers.emplace_back(dest, destSlicing);
    }

    CircuitConnection(decltype(destIdentifiers) &&lhsData,
                      std::unique_ptr<HDLExpressionAST> connAST) :
            ast(std::move(connAST)) {
        destIdentifiers = lhsData;
    }

    CircuitConnection(CircuitConnection &&conn) {
        destIdentifiers = conn.destIdentifiers;
        ast = std::move(conn.ast);
    }

    CircuitConnection &operator=(CircuitConnection &&conn) {
        destIdentifiers = conn.destIdentifiers;
        ast = std::move(conn.ast);
        return *this;
    }

    std::vector<std::pair<std::string, PortSlicingAST>> &getDestIdentifiers();

    [[nodiscard]] HDLExpressionAST *getHDLExpressionAST() const;
};

class NonBlockingAssignAST : public AlwaysBlockBodyAST {
public:
    CircuitConnection connection;

    explicit NonBlockingAssignAST(CircuitConnection &&conn) :
            AlwaysBlockBodyAST(nullptr), connection(std::move(conn)) {
        nodeType = "__hw_non_blk_assign__";
    }
};

class CircuitData {
public:
    std::vector<char> bits;
    PortSlicingAST slicing{0, 0};

    explicit CircuitData(const PortSlicingAST &slicingData);

    [[nodiscard]] std::size_t getBitWidth() const;

    [[nodiscard]] std::string toString() const;
};

class CircuitSymbol {
protected:
    std::string identifier;
    PortSlicingAST slicing;
    std::vector<std::pair<CircuitData, PortSlicingAST>> inputDataVec;
    std::vector<bool> inputReadyVec;
    std::vector<std::tuple<std::size_t, CircuitSymbol *, std::pair<bool, PortSlicingAST>>> propagateTargets;
    CircuitData outputData{PortSlicingAST{0, 0}};

    virtual CircuitData calculateOutput() = 0;

    virtual int getMaxInputs() = 0;

    [[nodiscard]] int getReadyInputs() const;

    CircuitData getInputCircuitData(std::size_t which);

    void resetReadyInputs();

    friend class CircuitSymbolWire;

public:
    explicit CircuitSymbol(std::string identifier) :
            identifier(std::move(identifier)),
            slicing(PortSlicingAST(0, 0)) {}

    [[nodiscard]] const PortSlicingAST &getSlicing() const;

    std::size_t registerInput(std::shared_ptr<CircuitSymbol> symbol,
                              const PortSlicingAST &inputSlicing = {-1, -1});

    virtual void propagate(std::size_t pos, const CircuitData &data);

    CircuitData getOutputData();

    [[nodiscard]] std::string getIdentifier() const;

    [[nodiscard]] const decltype(propagateTargets) &getPropagateTargets() const;

    virtual ~CircuitSymbol() = default;
};

class CircuitSymbolWire : public CircuitSymbol {
protected:
    CircuitData calculateOutput() override;

    int getMaxInputs() override;

public:
    explicit CircuitSymbolWire(std::string identifier,
                               const PortSlicingAST &slicingAst) :
            CircuitSymbol(std::move(identifier)) {
        slicing = slicingAst;
    }

    std::size_t registerInput(std::shared_ptr<CircuitSymbol> symbol,
                              const PortSlicingAST &destSlicing,
                              const PortSlicingAST &inputSlicing = {-1, -1});

    void propagate(std::size_t pos, const CircuitData &data) override;
};

class CircuitSymbolReg : public CircuitSymbolWire {
    CircuitData lastOutputData;
protected:
    CircuitData calculateOutput() override;

public:
    explicit CircuitSymbolReg(std::string identifier,
                              const PortSlicingAST &slicingAst) :
            CircuitSymbolWire(std::move(identifier), slicingAst),
            lastOutputData(slicingAst) {}
};

class CircuitSymbolConstant : public CircuitSymbolWire {
    static int counter;
    int value;
    int width;
protected:

    CircuitData calculateOutput() override;

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
        inputDataVec.emplace_back(CircuitData{slicing}, slicing);
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
            inputDataVec.emplace_back(CircuitData{slicing}, slicing);
            inputReadyVec.push_back(false);
        }
    }

    explicit ModuleIOPort(PortDirection direction, const PortSlicingAST &slicingAST,
                          std::string identifier) :
            CircuitSymbolWire(std::move(identifier), slicingAST),
            direction(direction) {
        slicing = slicingAST;
        if (direction == PortDirection::Input) {
            inputDataVec.emplace_back(CircuitData{slicing}, slicing);
            inputReadyVec.push_back(false);
        }
    }

    [[nodiscard]] PortDirection getPortDirection() const;

    void setPortDirection(PortDirection newDirection);
};

//class CircuitSymbolReg : public CircuitSymbol {
//public:
//    explicit CircuitSymbolReg(std::string identifier) : CircuitSymbol(std::move(identifier)) {}
//};

#endif //VERIPYTHON_CIRCUITSYMBOL_H
