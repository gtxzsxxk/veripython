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
        auto width = data0.getBitWidth();
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

#endif //VERIPYTHON_COMBLOGICS_H
