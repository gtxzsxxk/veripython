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

std::size_t CombLogic::generateUnsignedIntegerFromData(CircuitData *data) {
    std::size_t answer = 0;
    for (long int i = (long int) data->getBitWidth() - 1; i >= 0; i--) {
        auto bit = data->bits[i];
        if (bit) {
            answer |= 1;
        }
        answer <<= 1;
    }
    return answer;
}

CombLogic *CombLogicFactory::create(VeriPythonTokens token) {
    switch (token) {
        case TOKEN_logical_or:
            return new CombLogicLogicalOr;
        case TOKEN_logical_and:
            return new CombLogicLogicalAnd;
        case TOKEN_bitwise_or:
            return new CombLogicBitwiseOr;
        case TOKEN_bitwise_xor:
            return new CombLogicBitwiseXor;
        case TOKEN_bitwise_and:
            return new CombLogicBitwiseAnd;
        case TOKEN_cond_eq:
            return new CombLogicCompareEqual;
        case TOKEN_cond_ne:
            return new CombLogicCompareNonEqual;
        case TOKEN_cond_lt:
            return new CombLogicCompareLessThan;
        case TOKEN_cond_le:
            return new CombLogicCompareLessEqual;
        case TOKEN_cond_gt:
            return new CombLogicCompareGreatThan;
        case TOKEN_cond_ge:
            return new CombLogicCompareGreatEqual;
        case TOKEN_arith_lshift:
            return new CombLogicShiftLeftArith;
        case TOKEN_logical_lshift:
            return new CombLogicShiftLeftLogical;
        case TOKEN_arith_rshift:
            return new CombLogicShiftRightArith;
        case TOKEN_logical_rshift:
            return new CombLogicShiftRightLogical;
        case TOKEN_op_add:
            return new CombLogicArithAdd;
        case TOKEN_op_sub:
            return new CombLogicArithSub;
        case TOKEN_op_mod:
            return new CombLogicArithMod;
        case TOKEN_op_mul:
            return new CombLogicArithMul;
        case TOKEN_op_div:
            return new CombLogicArithDiv;
        case TOKEN_logical_not:
            return new CombLogicLogicalNot;
        case TOKEN_bitwise_not:
            return new CombLogicBitwiseNot;
        default:
            throw std::runtime_error("Unknown operator");
    }
}
