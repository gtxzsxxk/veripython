//
// Created by hanyuan on 2024/11/2.
//

#ifndef VERIPYTHON_AST_H
#define VERIPYTHON_AST_H

#include "Lexer.h"
#include <string>
#include <utility>
#include <vector>
#include <cstring>

class AST {
public:
    std::string nodeType;
    std::vector<AST *> children;

    explicit AST(std::string nodeType) : nodeType(std::move(nodeType)) {}

    virtual std::string toString();
};

class ConstantExpressionAST : public AST {
public:
    explicit ConstantExpressionAST(VeriPythonTokens operand) : AST("constantExpression"), operand(operand) {}

    VeriPythonTokens operand;

    virtual int eval();
};

class ConstantNumberAST : public ConstantExpressionAST {
public:
    explicit ConstantNumberAST(int value) : ConstantExpressionAST(TOKEN_number), value(value) {
        nodeType = "number";
    }

    int value = -1;

    std::string toString() override;

    int eval() override;
};

#endif //VERIPYTHON_AST_H
