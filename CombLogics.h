//
// Created by hanyuan on 2024/11/3.
//

#ifndef VERIPYTHON_COMBLOGICS_H
#define VERIPYTHON_COMBLOGICS_H

#include "HardwareModel.h"
#include "Parser.h"
#include <vector>
#include <stdexcept>

class CombLogic : public CircuitSymbol {
    static int counter;
protected:
    CircuitData calculateOutput() override = 0;

    int getMaxInputs() override = 0;

    static void checkInputDataSlicing(CircuitData *s1, CircuitData *s2);

public:
    explicit CombLogic(VeriPythonTokens _operator) : CircuitSymbol("") {
        if (Parser::operatorName.count(_operator) > 0) {
            identifier = "__comb_" + Parser::operatorName[_operator] + "__" + std::to_string(counter++);
        } else {
            throw std::runtime_error("Unsupported operator type");
        }
    }
};

template<class Op>
concept isLogicalBinOp = requires {
    Op::apply(true, true);
    std::same_as<decltype(TOKEN_logical_or), decltype(Op::token)>;
};

template<class Op> requires isLogicalBinOp<Op>
class CombLogicBinary : public CombLogic {
protected:
    CircuitData calculateOutput() override {
        auto data0 = inputDataVec[0];
        auto data1 = inputDataVec[1];
        checkInputDataSlicing(&data0, &data1);
        if (data0.getBitWidth() != 1) {
            throw std::runtime_error("Logical operation should only work with bit width 1");
        }
        auto slicing = PortSlicingAST(1);
        auto circuitData = CircuitData(slicing);
        circuitData.bits[0] = Op::apply(data0.bits[0], data1.bits[0]);
        return circuitData;
    }

    int getMaxInputs() override {
        return 2;
    }

public:
    CombLogicBinary() : CombLogic(Op::token) {}
};

/* Logical OR */
struct combOperatorLogicalOr {
    static constexpr auto token = TOKEN_logical_or;

    static bool apply(bool a, bool b) {
        return a || b;
    }
};

using CombLogicLogicalOr = CombLogicBinary<combOperatorLogicalOr>;

/* Logical AND */
struct combOperatorLogicalAnd {
    static constexpr auto token = TOKEN_logical_and;

    static bool apply(bool a, bool b) {
        return a && b;
    }
};

using CombLogicLogicalAnd = CombLogicBinary<combOperatorLogicalAnd>;

#endif //VERIPYTHON_COMBLOGICS_H
