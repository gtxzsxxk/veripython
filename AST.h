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
    explicit ConstantExpressionAST(VeriPythonTokens _operator) : AST("constantExpression"), _operator(_operator) {}

    VeriPythonTokens _operator;

    virtual int eval();
};

class ConstantNumberAST : public ConstantExpressionAST {
public:
    explicit ConstantNumberAST(int value) : ConstantExpressionAST(TOKEN_const_number), value(value) {
        nodeType = "const_number";
    }

    int value = -1;

    std::string toString() override;

    int eval() override;
};

class PortSlicingAST : public AST {
public:
    explicit PortSlicingAST(int high, int low) : AST("PortSlicing"), isDownTo(true), downToHigh(high), downToLow(low) {}

    explicit PortSlicingAST(int which) : AST("PortSlicing"), isDownTo(false), onlyWhich(which) {}

    bool isDownTo = false;
    int downToHigh = 0;
    int downToLow = 0;
    int onlyWhich = 0;
};

class HDLExpressionAST : public AST {
protected:
    PortSlicingAST *slicing = nullptr;
public:
    explicit HDLExpressionAST(VeriPythonTokens _operator) : AST("hdlExpression"), _operator(_operator) {}

    VeriPythonTokens _operator;

    void setSlicing(PortSlicingAST *slicingAst);

    std::string toString() override;
};

class HDLPrimaryAST : public HDLExpressionAST {
    bool isIdentifier = false;
    int base = 10;
    int width = 32;
    int value = 0;
    std::string identifier;
public:
    explicit HDLPrimaryAST(int constantNumber) :
            HDLExpressionAST(TOKEN_const_number),
            isIdentifier(false), value(constantNumber) {
        nodeType = "const_number";
    }

    explicit HDLPrimaryAST(int constantNumber,
                           int width, int base) :
            HDLExpressionAST(TOKEN_sized_number),
            isIdentifier(false),
            base(base),
            width(width),
            value(constantNumber) {
        nodeType = "const_number";
    }

    explicit HDLPrimaryAST(std::string identifier) :
            HDLExpressionAST(TOKEN_identifier),
            isIdentifier(true), identifier(std::move(identifier)) {
        nodeType = "identifier";
    }

    std::string toString() override;
};

#endif //VERIPYTHON_AST_H
