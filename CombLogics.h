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

/*
 * Normal Binary Operators
 * */

template<class Op>
concept isNormalBinOp = requires {
    Op::apply(true, true);
    std::same_as<decltype(TOKEN_logical_or), decltype(Op::token)>;
};

template<class Op> requires isNormalBinOp<Op>
class CombNormalBinary : public CombLogic {
protected:
    CircuitData calculateOutput() override {
        auto data0 = inputDataVec[0];
        auto data1 = inputDataVec[1];
        checkInputDataSlicing(&data0, &data1);
        if constexpr (Op::token == TOKEN_logical_or || TOKEN_logical_and) {
            if (data0.getBitWidth() != 1) {
                throw std::runtime_error("Logical operation should only work with bit width 1");
            }
        }
        auto width = data0.getBitWidth();
        auto slicing = PortSlicingAST(width);
        auto circuitData = CircuitData(slicing);
        for (auto i = 0; i < width; i++) {
            circuitData.bits[i] = Op::apply(data0.bits[i], data1.bits[i]);
        }
        return circuitData;
    }

    int getMaxInputs() override {
        return 2;
    }

public:
    CombNormalBinary() : CombLogic(Op::token) {}
};

/* Logical OR */
struct combOperatorLogicalOr {
    static constexpr auto token = TOKEN_logical_or;

    static bool apply(bool a, bool b) {
        return a || b;
    }
};

using CombLogicLogicalOr = CombNormalBinary<combOperatorLogicalOr>;

/* Logical AND */
struct combOperatorLogicalAnd {
    static constexpr auto token = TOKEN_logical_and;

    static bool apply(bool a, bool b) {
        return a && b;
    }
};

using CombLogicLogicalAnd = CombNormalBinary<combOperatorLogicalAnd>;

/* Bitwise OR */
struct combOperatorBitwiseOr {
    static constexpr auto token = TOKEN_bitwise_or;

    static bool apply(bool a, bool b) {
        return a || b;
    }
};

using CombLogicBitwiseOr = CombNormalBinary<combOperatorBitwiseOr>;

/* Bitwise XOR */
struct combOperatorBitwiseXor {
    static constexpr auto token = TOKEN_bitwise_xor;

    static bool apply(bool a, bool b) {
        return a ^ b;
    }
};

using CombLogicBitwiseXor = CombNormalBinary<combOperatorBitwiseXor>;

/* Bitwise AND */
struct combOperatorBitwiseAnd {
    static constexpr auto token = TOKEN_bitwise_and;

    static bool apply(bool a, bool b) {
        return a && b;
    }
};

using CombLogicBitwiseAnd = CombNormalBinary<combOperatorBitwiseAnd>;

#endif //VERIPYTHON_COMBLOGICS_H
