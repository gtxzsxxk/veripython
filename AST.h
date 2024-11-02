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

    int eval();
};

class NumberAST : public AST {
public:
    explicit NumberAST(int value) : AST("number"), value(value) {}

    int value = -1;

    std::string toString() override;
};

#endif //VERIPYTHON_AST_H
