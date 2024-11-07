//
// Created by hanyuan on 2024/11/3.
//

#ifndef VERIPYTHON_COMBLOGICS_H
#define VERIPYTHON_COMBLOGICS_H

#include "RtlModule.h"
#include "Parser.h"
#include <tuple>
#include <vector>
#include <stdexcept>
#include <memory>

class CombLogic : public CircuitSymbol {
    static int counter;
protected:
    CircuitData calculateOutput() override = 0;

    int getMaxInputs() override = 0;

    static void checkInputDataSlicing(CircuitData *s1, CircuitData *s2);

    static std::tuple<bool, std::size_t> generateUnsignedIntegerFromData(CircuitData *data);

public:
    explicit CombLogic(VeriPythonTokens _operator) : CircuitSymbol("") {
        if (Parser::operatorName.count(_operator) > 0) {
            identifier = "__comb_" + Parser::operatorName[_operator] + "__" + std::to_string(counter++);
        } else {
            throw std::runtime_error("Unsupported operator type");
        }
    }

    virtual ~CombLogic() = default;
};

/*
 * Normal Binary Operators
 * */

template<class Op>
concept isNormalBinOp = requires {
    Op::apply(1, 1);
    std::same_as<decltype(TOKEN_logical_or), decltype(Op::token)>;
};

template<class Op> requires isNormalBinOp<Op>
class CombNormalBinary : public CombLogic {
protected:
    CircuitData calculateOutput() override {
        auto data0 = getInputCircuitData(0);
        auto data1 = getInputCircuitData(1);
        checkInputDataSlicing(&data0, &data1);
        if constexpr (Op::token == TOKEN_logical_or || Op::token == TOKEN_logical_and) {
            if (data0.getBitWidth() != 1) {
                throw std::runtime_error("Logical operation should only work with bit width 1");
            }
        }
        auto width = data0.getBitWidth();
        auto slicing = PortSlicingAST(width - 1, 0);
        auto circuitData = CircuitData(slicing);
        for (decltype(width) i = 0; i < width; i++) {
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
                                                                static char apply(char a, char b) {
#define GEN_LOGICAL_OP_DEF_END(camel)                } \
                                                            }; \
                                                            using CombLogic##camel = CombNormalBinary<combOperator##camel>;


GEN_LOGICAL_OP_DEF_BEGIN(LogicalOr, logical_or)
        return (char) ((a == -1 || b == -1) ? -1 : (a || b));
GEN_LOGICAL_OP_DEF_END(LogicalOr)

GEN_LOGICAL_OP_DEF_BEGIN(LogicalAnd, logical_and)
        return (char) ((a == -1 || b == -1) ? -1 : (a && b));
GEN_LOGICAL_OP_DEF_END(LogicalAnd)

GEN_LOGICAL_OP_DEF_BEGIN(BitwiseOr, bitwise_or)
        return (char) ((a == -1 || b == -1) ? -1 : (a || b));
GEN_LOGICAL_OP_DEF_END(BitwiseOr)

GEN_LOGICAL_OP_DEF_BEGIN(BitwiseXor, bitwise_xor)
        return (char) ((a == -1 || b == -1) ? -1 : (a ^ b));
GEN_LOGICAL_OP_DEF_END(BitwiseXor)

GEN_LOGICAL_OP_DEF_BEGIN(BitwiseAnd, bitwise_and)
        return (char) ((a == -1 || b == -1) ? -1 : (a && b));
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
        auto data0 = getInputCircuitData(0);
        auto data1 = getInputCircuitData(1);
        checkInputDataSlicing(&data0, &data1);
        auto slicing = PortSlicingAST(0, 0);
        auto circuitData = CircuitData(slicing);
        auto [avail1, cmp1] = generateUnsignedIntegerFromData(&data0);
        auto [avail2, cmp2] = generateUnsignedIntegerFromData(&data1);
        if (avail1 && avail2) {
            circuitData.bits[0] = Op::apply(cmp1, cmp2);
        } else {
            circuitData.bits[0] = -1;
        }
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
                                                                static char apply(std::size_t a, std::size_t b) {
#define GEN_COMPARE_OP_DEF_END(camel)                           } \
                                                            }; \
                                                            using CombLogic##camel = CombCompareBinary<combOperator##camel>;


GEN_COMPARE_OP_DEF_BEGIN(CompareEqual, cond_eq)
        return a == b;
GEN_COMPARE_OP_DEF_END(CompareEqual)

GEN_COMPARE_OP_DEF_BEGIN(CompareNonEqual, cond_ne)
        return a != b;
GEN_COMPARE_OP_DEF_END(CompareNonEqual)

GEN_COMPARE_OP_DEF_BEGIN(CompareLessThan, cond_lt)
        return a < b;
GEN_COMPARE_OP_DEF_END(CompareLessThan)

GEN_COMPARE_OP_DEF_BEGIN(CompareLessEqual, cond_le)
        return a <= b;
GEN_COMPARE_OP_DEF_END(CompareLessEqual)

GEN_COMPARE_OP_DEF_BEGIN(CompareGreatThan, cond_gt)
        return a > b;
GEN_COMPARE_OP_DEF_END(CompareGreatThan)

GEN_COMPARE_OP_DEF_BEGIN(CompareGreatEqual, cond_ge)
        return a >= b;
GEN_COMPARE_OP_DEF_END(CompareGreatEqual)

/*
 * Left Shift
 * */

template<class Op> requires requires { std::same_as<decltype(TOKEN_logical_or), decltype(Op::token)>; }
class CombShiftLeft : public CombLogic {
protected:
    CircuitData calculateOutput() override {
        auto data0 = getInputCircuitData(0);
        auto data1 = getInputCircuitData(1);

        auto [avail, shiftAmount] = generateUnsignedIntegerFromData(&data1);

        auto width = data0.getBitWidth();
        if (avail) {
            auto slicing = PortSlicingAST(width + shiftAmount - 1, 0);
            auto circuitData = CircuitData(slicing);

            for (long int i = (long int) circuitData.bits.size() - 1;
                 i >= static_cast<long int>(circuitData.bits.size() - width); i--) {
                circuitData.bits[i] = data0.bits[i - shiftAmount];
            }
            for (long int i = circuitData.bits.size() - width - 1; i >= 0; i--) {
                circuitData.bits[i] = 0;
            }
            return circuitData;
        } else {
            auto slicing = PortSlicingAST(width - 1, 0);
            auto circuitData = CircuitData(slicing);
            for (unsigned long i = 0; i < width; i++) {
                circuitData.bits[i] = -1;
            }
            return circuitData;
        }
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
        auto data0 = getInputCircuitData(0);
        auto data1 = getInputCircuitData(1);

        auto [avail, shiftAmount] = generateUnsignedIntegerFromData(&data1);

        auto width = data0.getBitWidth();
        if (avail) {
            auto slicing = PortSlicingAST(width - 1, 0);
            auto circuitData = CircuitData(slicing);

            char signExtending = 0;
            if constexpr (Op::token == TOKEN_arith_rshift) {
                signExtending = data0.bits[width - 1];
            }

            for (long int i = width - 1; i >= static_cast<long int>(width - shiftAmount); i--) {
                circuitData.bits[i] = signExtending;
            }
            for (long int i = width - shiftAmount - 1; i >= 0; i--) {
                circuitData.bits[i] = data0.bits[i + shiftAmount];
            }
            return circuitData;
        } else {
            auto slicing = PortSlicingAST(width - 1, 0);
            auto circuitData = CircuitData(slicing);
            for (unsigned long i = 0; i < width; i++) {
                circuitData.bits[i] = -1;
            }
            return circuitData;
        }
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
        auto data0 = getInputCircuitData(0);
        auto data1 = getInputCircuitData(1);

        decltype(data0.getBitWidth()) width;
        if constexpr (Op::token == TOKEN_op_mul) {
            width = data0.getBitWidth() + data1.getBitWidth();
        } else if constexpr (Op::token == TOKEN_op_mod) {
            width = std::min(data0.getBitWidth(), data1.getBitWidth());
        } else if constexpr (Op::token == TOKEN_op_div) {
            width = data0.getBitWidth();
        } else {
            width = std::max(data0.getBitWidth(), data1.getBitWidth());
        }

        auto slicing = PortSlicingAST(width - 1, 0);
        auto circuitData = CircuitData(slicing);

        auto [avail1, operand1] = generateUnsignedIntegerFromData(&data0);
        auto [avail2, operand2] = generateUnsignedIntegerFromData(&data1);

        if (avail1 && avail2) {
            std::size_t ans = Op::apply(operand1, operand2);

            for (decltype(width) i = 0; i < width; i++) {
                circuitData.bits[i] = (ans & 0x01UL) == 1;
                ans >>= 1;
            }
        } else {
            for (decltype(width) i = 0; i < width; i++) {
                circuitData.bits[i] = -1;
            }
        }
        return circuitData;
    }

    int getMaxInputs() override {
        return 2;
    }

public:
    CombArithmeticBinary() : CombLogic(Op::token) {}
};

#define GEN_ARITH_OP_DEF_BEGIN(camel, underline)          struct combOperator##camel { \
                                                                static constexpr auto token = TOKEN_##underline; \
                                                                static std::size_t apply(std::size_t a, std::size_t b) {
#define GEN_ARITH_OP_DEF_END(camel)                } \
                                                            }; \
                                                            using CombLogic##camel = CombArithmeticBinary<combOperator##camel>;

GEN_ARITH_OP_DEF_BEGIN(ArithAdd, op_add)
        return a + b;
GEN_ARITH_OP_DEF_END(ArithAdd)

GEN_ARITH_OP_DEF_BEGIN(ArithSub, op_sub)
        return a - b;
GEN_ARITH_OP_DEF_END(ArithSub)

GEN_ARITH_OP_DEF_BEGIN(ArithMod, op_mod)
        return a % b;
GEN_ARITH_OP_DEF_END(ArithMod)

GEN_ARITH_OP_DEF_BEGIN(ArithMul, op_mul)
        return a * b;
GEN_ARITH_OP_DEF_END(ArithMul)

GEN_ARITH_OP_DEF_BEGIN(ArithDiv, op_div)
        return a / b;
GEN_ARITH_OP_DEF_END(ArithDiv)

/*
 * Unary Operators
 * */
template<class Op>
concept isUnaryOp = requires {
    Op::apply(1);
    std::same_as<decltype(TOKEN_logical_or), decltype(Op::token)>;
};

template<class Op> requires isUnaryOp<Op>
class CombUnary : public CombLogic {
protected:
    CircuitData calculateOutput() override {
        auto data0 = getInputCircuitData(0);
        auto width = data0.getBitWidth();

        if constexpr (Op::token == TOKEN_logical_not) {
            if (data0.getBitWidth() != 1) {
                throw std::runtime_error("Logical operation should only work with bit width 1");
            }
        }

        auto slicing = PortSlicingAST(width - 1, 0);
        auto circuitData = CircuitData(slicing);

        for (decltype(width) i = 0; i < width; i++) {
            circuitData.bits[i] = Op::apply(data0.bits[i]);
        }
        return circuitData;
    }

    int getMaxInputs() override {
        return 1;
    }

public:
    CombUnary() : CombLogic(Op::token) {}
};

struct combUnaryLogicalNot {
    static constexpr auto token = TOKEN_logical_not;

    static char apply(char a) {
        return (char) (a == -1 ? -1 : !a);
    }
};

using CombLogicLogicalNot = CombUnary<combUnaryLogicalNot>;

struct combUnaryBitwiseNot {
    static constexpr auto token = TOKEN_bitwise_not;

    static char apply(char a) {
        return (char) (a == -1 ? -1 : !a);
    }
};

using CombLogicBitwiseNot = CombUnary<combUnaryBitwiseNot>;

class CombLogicMultiplexer : public CombLogic {
protected:
    CircuitData calculateOutput() override {
        auto selData = getInputCircuitData(0);
        auto data0 = getInputCircuitData(1);
        auto data1 = getInputCircuitData(2);

        bool unknownState = false;
        bool selData0 = false;
        for (const auto &bit: selData.bits) {
            if (bit == -1) {
                unknownState = true;
                break;
            } else {
                if (bit == 1) {
                    selData0 = true;
                    break;
                }
            }
        }

        decltype(data0.getBitWidth()) width;

        if (selData0) {
            width = data0.getBitWidth();
        } else {
            width = data1.getBitWidth();
        }

        auto slicing = PortSlicingAST((int) width - 1, 0);
        auto circuitData = CircuitData(slicing);

        if (unknownState) {
            for (decltype(width) i = 0; i < width; i++) {
                circuitData.bits[i] = -1;
            }
        } else {
            if (selData0) {
                for (decltype(width) i = 0; i < width; i++) {
                    circuitData.bits[i] = data0.bits[i];
                }
            } else {
                for (decltype(width) i = 0; i < width; i++) {
                    circuitData.bits[i] = data1.bits[i];
                }
            }
        }
        return circuitData;
    }

    int getMaxInputs() override {
        return 3;
    }

public:
    CombLogicMultiplexer() : CombLogic(TOKEN_question) {}
};

class CombLogicFactory {
public:
    static std::unique_ptr<CombLogic> create(VeriPythonTokens token);
};

#endif //VERIPYTHON_COMBLOGICS_H
