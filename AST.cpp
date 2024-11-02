//
// Created by hanyuan on 2024/11/2.
//
#include "AST.h"
#include <stdexcept>

std::string AST::toString() {
    std::string xmlOutput = "<" + nodeType + ">\n";
    for (auto *ast: children) {
        std::string tmp = ast->toString();
        char *childOutput = strdup(tmp.c_str());
        for (char *line = strtok(childOutput, "\n"); line; line = strtok(nullptr, "\n")) {
            xmlOutput += "    " + std::string{line} + "\n";
        }
        free(childOutput);
    }
    xmlOutput += "</" + nodeType + ">\n";
    return xmlOutput;
}

int ConstantExpressionAST::eval() {
    if (children.size() != 2) {
        throw std::runtime_error("Cannot eval an expression node");
    }
    int lhsValue = dynamic_cast<ConstantExpressionAST *>(children[0])->eval();
    int rhsValue = dynamic_cast<ConstantExpressionAST *>(children[1])->eval();
    int answer;
    switch (operand) {
        case TOKEN_op_add:
            answer = lhsValue + rhsValue;
            break;
        case TOKEN_op_sub:
            answer = lhsValue - rhsValue;
            break;
        case TOKEN_op_mul:
            answer = lhsValue * rhsValue;
            break;
        case TOKEN_op_div:
            answer = lhsValue / rhsValue;
            break;
        default:
            throw std::runtime_error("Unsupported operand");
            break;
    }
    return answer;
}

std::string ConstantNumberAST::toString() {
    return "<" + nodeType + ">\n    " + std::to_string(value) + "\n</" + nodeType + ">\n";
}

int ConstantNumberAST::eval() {
    return value;
}
