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

    static std::size_t generateUnsignedIntegerFromData(CircuitData *data);

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

#define GEN_LOGICAL_OP_DEF_BEGIN(camel, underline)          struct combOperator##camel { \
                                                                static constexpr auto token = TOKEN_##underline; \
                                                                static bool apply(bool a, bool b) {
#define GEN_LOGICAL_OP_DEF_END(camel)                } \
                                                            }; \
                                                            using CombLogic##camel = CombNormalBinary<combOperator##camel>;


GEN_LOGICAL_OP_DEF_BEGIN(LogicalOr, logical_or)
        return a || b;
GEN_LOGICAL_OP_DEF_END(LogicalOr)

GEN_LOGICAL_OP_DEF_BEGIN(LogicalAnd, logical_and)
        return a && b;
GEN_LOGICAL_OP_DEF_END(LogicalAnd)

GEN_LOGICAL_OP_DEF_BEGIN(BitwiseOr, bitwise_or)
        return a || b;
GEN_LOGICAL_OP_DEF_END(BitwiseOr)

GEN_LOGICAL_OP_DEF_BEGIN(BitwiseXor, bitwise_xor)
        return a ^ b;
GEN_LOGICAL_OP_DEF_END(BitwiseXor)

GEN_LOGICAL_OP_DEF_BEGIN(BitwiseAnd, bitwise_and)
        return a && b;
GEN_LOGICAL_OP_DEF_END(BitwiseAnd)

/*
 * Compare Binary Operators
 * */
template<class Op>
concept isCompareBinOp = requires {
    Op::apply(114UL, 514UL);
    std::same_as<decltype(TOKEN_logical_or), decltype(Op::token)>;
};

template<class Op> requires isCompareBinOp<Op>
class CombCompareBinary : public CombLogic {
protected:
    CircuitData calculateOutput() override {
        auto data0 = inputDataVec[0];
        auto data1 = inputDataVec[1];
        checkInputDataSlicing(&data0, &data1);
        auto slicing = PortSlicingAST(1);
        auto circuitData = CircuitData(slicing);
        auto cmp1 = generateUnsignedIntegerFromData(data0);
        auto cmp2 = generateUnsignedIntegerFromData(data1);
        circuitData.bits[0] = Op::apply(cmp1, cmp2);
        return circuitData;
    }

    int getMaxInputs() override {
        return 2;
    }

public:
    CombCompareBinary() : CombLogic(Op::token) {}
};

#define GEN_COMPARE_OP_DEF_BEGIN(camel, underline)          struct combOperator##camel { \
                                                                static constexpr auto token = TOKEN_##underline; \
                                                                static bool apply(std::size_t a, std::size_t b) {
#define GEN_COMPARE_OP_DEF_END(camel)                           } \
                                                            }; \
                                                            using CombLogic##camel = CombNormalBinary<combOperator##camel>;


GEN_LOGICAL_OP_DEF_BEGIN(CompareEqual, cond_eq)
        return a == b;
GEN_LOGICAL_OP_DEF_END(CompareEqual)

GEN_LOGICAL_OP_DEF_BEGIN(CompareNonEqual, cond_ne)
        return a != b;
GEN_LOGICAL_OP_DEF_END(CompareNonEqual)

GEN_LOGICAL_OP_DEF_BEGIN(CompareLessThan, cond_lt)
        return a < b;
GEN_LOGICAL_OP_DEF_END(CompareLessThan)

GEN_LOGICAL_OP_DEF_BEGIN(CompareLessEqual, cond_le)
        return a <= b;
GEN_LOGICAL_OP_DEF_END(CompareLessEqual)

GEN_LOGICAL_OP_DEF_BEGIN(CompareGreatThan, cond_gt)
        return a > b;
GEN_LOGICAL_OP_DEF_END(CompareGreatThan)

GEN_LOGICAL_OP_DEF_BEGIN(CompareGreatEqual, cond_ge)
        return a >= b;
GEN_LOGICAL_OP_DEF_END(CompareGreatEqual)

/*
 * Left Shift
 * */

template<class Op> requires requires { std::same_as<decltype(TOKEN_logical_or), decltype(Op::token)>; }
class CombShiftLeft : public CombLogic {
protected:
    CircuitData calculateOutput() override {
        auto data0 = inputDataVec[0];
        auto data1 = inputDataVec[1];

        auto shiftAmount = generateUnsignedIntegerFromData(data1);

        auto width = data0.getBitWidth();
        auto slicing = PortSlicingAST(width + shiftAmount);
        auto circuitData = CircuitData(slicing);

        for (long int i = (long int) circuitData.bits.size() - 1; i >= circuitData.bits.size() - width; i--) {
            circuitData.bits[i] = data0.bits[i - shiftAmount];
        }
        for (long int i = circuitData.bits.size() - width - 1; i >= 0; i--) {
            circuitData.bits[i] = false;
        }
        return circuitData;
    }

    int getMaxInputs() override {
        return 2;
    }

public:
    CombShiftLeft() : CombLogic(Op::token) {}
};

struct combOperatorShiftLeftArith {
    static constexpr auto token = TOKEN_arith_lshift;
};

using CombLogicShiftLeftArith = CombShiftLeft<combOperatorShiftLeftArith>;

struct combOperatorShiftLeftLogical {
    static constexpr auto token = TOKEN_logical_lshift;
};

using CombLogicShiftLeftLogical = CombShiftLeft<combOperatorShiftLeftLogical>;

/*
 * Right Shift
 * */
template<class Op> requires requires { std::same_as<decltype(TOKEN_logical_or), decltype(Op::token)>; }
class CombShiftRight : public CombLogic {
protected:
    CircuitData calculateOutput() override {
        auto data0 = inputDataVec[0];
        auto data1 = inputDataVec[1];

        auto shiftAmount = generateUnsignedIntegerFromData(data1);

        auto width = data0.getBitWidth();
        auto slicing = PortSlicingAST(width);
        auto circuitData = CircuitData(slicing);

        bool signExtending = false;
        if constexpr (Op::token == TOKEN_arith_rshift) {
            signExtending = data0.bits[width - 1];
        }

        for (long int i = width - 1; i >= width - shiftAmount; i--) {
            circuitData.bits[i] = signExtending;
        }
        for (long int i = width - shiftAmount - 1; i >= 0; i--) {
            circuitData.bits[i] = data0[i + shiftAmount];
        }
        return circuitData;
    }

    int getMaxInputs() override {
        return 2;
    }

public:
    CombShiftRight() : CombLogic(Op::token) {}
};

struct combOperatorShiftRightArith {
    static constexpr auto token = TOKEN_arith_rshift;
};

using CombLogicShiftRightArith = CombShiftRight<combOperatorShiftRightArith>;

struct combOperatorShiftRightLogical {
    static constexpr auto token = TOKEN_logical_rshift;
};

using CombLogicShiftRightLogical = CombShiftRight<combOperatorShiftRightLogical>;

/*
 * Arithmetic Binary Operators
 * */
template<class Op> requires isCompareBinOp<Op>
class CombArithmeticBinary : public CombLogic {
protected:
    CircuitData calculateOutput() override {
        auto data0 = inputDataVec[0];
        auto data1 = inputDataVec[1];

        auto width = std::max(data0.getBitWidth(), data1.getBitWidth());

        auto slicing = PortSlicingAST(width);
        auto circuitData = CircuitData(slicing);

        auto operand1 = generateUnsignedIntegerFromData(data0);
        auto operand2 = generateUnsignedIntegerFromData(data1);

        std::size_t ans = Op::apply(operand1, operand2);

        for (auto i = 0; i < width; i++) {
            circuitData.bits[i] = (ans & 0x01UL) == 1;
            ans >>= 1;
        }
        return circuitData;
    }

    int getMaxInputs() override {
        return 2;
    }

public:
    CombArithmeticBinary() : CombLogic(Op::token) {}
};

#endif //VERIPYTHON_COMBLOGICS_H
