//
// Created by hanyuan on 2024/11/3.
//

#include "CombLogics.h"
#include <stdexcept>

int CombLogic::counter = 0;

void CombLogic::checkInputDataSlicing(CircuitData *s1, CircuitData *s2) {
    auto slicing1 = s1->getBitWidth();
    auto slicing2 = s2->getBitWidth();
    if (slicing1 != slicing2) {
        throw std::runtime_error("Width mismatched");
    }
}

std::tuple<bool, std::size_t> CombLogic::generateUnsignedIntegerFromData(CircuitData *data) {
    std::size_t answer = 0;
    for (long int i = (long int) data->getBitWidth() - 1; i >= 0; i--) {
        auto bit = data->bits[i];
        if (bit == 1) {
            answer |= 1;
        } else if (bit == 0) {
            answer |= 0;
        } else {
            return {false, 0};
        }
        if (i) {
            answer <<= 1;
        }
    }
    return {true, answer};
}

std::unique_ptr<CombLogic> CombLogicFactory::create(VeriPythonTokens token) {
    switch (token) {
        case TOKEN_logical_or:
            return std::make_unique<CombLogicLogicalOr>();
        case TOKEN_logical_and:
            return std::make_unique<CombLogicLogicalAnd>();
        case TOKEN_bitwise_or:
            return std::make_unique<CombLogicBitwiseOr>();
        case TOKEN_bitwise_xor:
            return std::make_unique<CombLogicBitwiseXor>();
        case TOKEN_bitwise_and:
            return std::make_unique<CombLogicBitwiseAnd>();
        case TOKEN_cond_eq:
            return std::make_unique<CombLogicCompareEqual>();
        case TOKEN_cond_ne:
            return std::make_unique<CombLogicCompareNonEqual>();
        case TOKEN_cond_lt:
            return std::make_unique<CombLogicCompareLessThan>();
        case TOKEN_cond_le:
            return std::make_unique<CombLogicCompareLessEqual>();
        case TOKEN_cond_gt:
            return std::make_unique<CombLogicCompareGreatThan>();
        case TOKEN_cond_ge:
            return std::make_unique<CombLogicCompareGreatEqual>();
        case TOKEN_arith_lshift:
            return std::make_unique<CombLogicShiftLeftArith>();
        case TOKEN_logical_lshift:
            return std::make_unique<CombLogicShiftLeftLogical>();
        case TOKEN_arith_rshift:
            return std::make_unique<CombLogicShiftRightArith>();
        case TOKEN_logical_rshift:
            return std::make_unique<CombLogicShiftRightLogical>();
        case TOKEN_op_add:
            return std::make_unique<CombLogicArithAdd>();
        case TOKEN_op_sub:
            return std::make_unique<CombLogicArithSub>();
        case TOKEN_op_mod:
            return std::make_unique<CombLogicArithMod>();
        case TOKEN_op_mul:
            return std::make_unique<CombLogicArithMul>();
        case TOKEN_op_div:
            return std::make_unique<CombLogicArithDiv>();
        case TOKEN_logical_not:
            return std::make_unique<CombLogicLogicalNot>();
        case TOKEN_bitwise_not:
            return std::make_unique<CombLogicBitwiseNot>();
        case TOKEN_question:
            return std::make_unique<CombLogicMultiplexer>();
        default:
            throw std::runtime_error("Unknown operator");
    }
}
