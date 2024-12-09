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

class CircuitConnection {
    std::vector<std::pair<std::string, PortSlicingAST>> destIdentifiers;
public:
    std::pair<TriggerEdgeType, std::string> clocking{TriggerEdgeType::NOT_SPECIFIED, {}};
    std::unique_ptr<HDLExpressionAST> ast;

    CircuitConnection(const std::string &dest,
                      std::unique_ptr<HDLExpressionAST> connAST) :
            ast(std::move(connAST)) {
        destIdentifiers.emplace_back(dest, PortSlicingAST{-1, -1});
    }

    CircuitConnection(const std::string &dest, const PortSlicingAST &destSlicing,
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
        clocking = conn.clocking;
        ast = std::move(conn.ast);
    }

    CircuitConnection &operator=(CircuitConnection &&conn) {
        destIdentifiers = conn.destIdentifiers;
        clocking = conn.clocking;
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
    bool isRegister = false;
    std::string identifier;
    PortSlicingAST slicing;
    std::vector<std::pair<CircuitData, PortSlicingAST>> inputDataVec;
    /* backward symbol, input slicing, dest slicing */
    std::vector<std::tuple<std::shared_ptr<CircuitSymbol>, PortSlicingAST, PortSlicingAST>> backwardSymbols;
    CircuitData outputData{PortSlicingAST{0, 0}};
    std::vector<std::tuple<std::size_t, CircuitSymbol *, PortSlicingAST>> propagateTargets;

    virtual CircuitData calculateOutput() = 0;

    virtual int getMaxInputs() = 0;

    CircuitData getInputCircuitData(std::size_t which);

    friend class CircuitSymbolWire;

    friend class CircuitSymbolReg;

public:
    explicit CircuitSymbol(std::string identifier) :
            identifier(std::move(identifier)),
            slicing(PortSlicingAST(0, 0)) {}

    [[nodiscard]] const PortSlicingAST &getSlicing() const;

    std::size_t registerInput(const std::shared_ptr<CircuitSymbol> &symbol,
                              const PortSlicingAST &inputSlicing = {-1, -1});

    virtual void propagate(std::size_t pos, const CircuitData &data);

    CircuitData getOutputData();

    [[nodiscard]] std::string getIdentifier() const;

    [[nodiscard]] const decltype(propagateTargets) &getPropagateTargets() const;

    [[nodiscard]] const decltype(backwardSymbols) &getBackwardSymbols() const;

    [[nodiscard]] bool isRegisterSymbol() const;

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

    std::size_t registerInput(const std::shared_ptr<CircuitSymbol> &symbol,
                              const PortSlicingAST &destSlicing,
                              const PortSlicingAST &inputSlicing = {-1, -1});

    void propagate(std::size_t pos, const CircuitData &data) override;
};

class CircuitSymbolReg : public CircuitSymbolWire {
    PortSlicingAST clockSlicing{0, 0};
    CircuitData storedData;
    CircuitData prevClockSignal{clockSlicing};
    TriggerEdgeType triggerType{TriggerEdgeType::NOT_SPECIFIED};
protected:
    CircuitData calculateOutput() override;

    int getMaxInputs() override;

public:
    explicit CircuitSymbolReg(std::string identifier,
                              const PortSlicingAST &slicingAst) :
            CircuitSymbolWire(std::move(identifier), slicingAst),
            storedData(slicingAst) {
        isRegister = true;
    }

    std::size_t registerClock(const std::shared_ptr<CircuitSymbol> &symbol);

    void propagate(std::size_t pos, const CircuitData &data) override;

    void propagateStoredData();

    void setTriggerType(TriggerEdgeType type);
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
    }

    [[nodiscard]] decltype(value) getValue() const { return value; }
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
        }
    }

    explicit ModuleIOPort(PortDirection direction, const PortSlicingAST &slicingAST,
                          std::string identifier) :
            CircuitSymbolWire(std::move(identifier), slicingAST),
            direction(direction) {
        slicing = slicingAST;
        if (direction == PortDirection::Input) {
            inputDataVec.emplace_back(CircuitData{slicing}, slicing);
        }
    }

    [[nodiscard]] PortDirection getPortDirection() const;

    void setPortDirection(PortDirection newDirection);
};

#endif //VERIPYTHON_CIRCUITSYMBOL_H
