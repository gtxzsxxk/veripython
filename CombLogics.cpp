//
// Created by hanyuan on 2024/11/3.
//

#include "CombLogics.h"
#include <stdexcept>

int CombLogic::counter = 0;

void CombLogic::checkInputDataSlicing(CircuitData *s1, CircuitData *s2) {
    auto slicing1 = s1->getBitWidth();
    auto slicing2 = s2->getBitWidth();
    if(slicing1 != slicing2) {
        throw std::runtime_error("Width mismatched");
    }
}

std::size_t CombLogic::generateUnsignedIntegerFromData(CircuitData *data) {
    std::size_t answer = 0;
    for(long int i = (long int) data->getBitWidth() - 1; i >= 0; i--) {
        auto bit = data->bits[i];
        if(bit) {
            answer |= 1;
        }
        answer <<= 1;
    }
    return answer;
}
