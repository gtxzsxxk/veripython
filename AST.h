//
// Created by hanyuan on 2024/11/2.
//

#ifndef VERIPYTHON_AST_H
#define VERIPYTHON_AST_H

#include "Lexer.h"
#include "SequentialLogics.h"
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include <cstring>

class AST {
public:
    std::string nodeType;
    std::vector<std::unique_ptr<AST>> children;

    explicit AST(std::string nodeType) : nodeType(std::move(nodeType)) {}

    virtual std::string toString();

    virtual ~AST() = default;
};

class ConstantExpressionAST : public AST {
public:
    explicit ConstantExpressionAST(VeriPythonTokens _operator) :
            AST("constantExpression"),
            _operator(_operator) {}

    VeriPythonTokens _operator;

    virtual int eval();
};

class ConstantNumberAST : public ConstantExpressionAST {
public:
    explicit ConstantNumberAST(int value) :
            ConstantExpressionAST(TOKEN_const_number),
            value(value) {
        nodeType = "const_number";
    }

    int value = -1;

    std::string toString() override;

    int eval() override;
};

class PortSlicingAST : public AST {
public:
    bool isDownTo = false;
    int downToHigh = 0;
    int downToLow = 0;
    int onlyWhich = 0;

    PortSlicingAST(int high, int low) :
            AST("PortSlicing"), isDownTo(true),
            downToHigh(high), downToLow(low) {}

    explicit PortSlicingAST(int which) :
            AST("PortSlicing"),
            isDownTo(false),
            onlyWhich(which) {}

    PortSlicingAST(const PortSlicingAST &slicingAST);

    PortSlicingAST &operator=(const PortSlicingAST &slicingAST);

    [[nodiscard]] bool isTrivial() const;
};

class HDLExpressionAST : public AST {
public:
    PortSlicingAST exprSlicing{-1, -1};
    VeriPythonTokens _operator;

    explicit HDLExpressionAST(VeriPythonTokens _operator) :
            AST("hdlExpression"), _operator(_operator) {}

    void setExprSlicing(const PortSlicingAST &slicingAst);

    std::string toString() override;

    static bool canParseToCombLogics(VeriPythonTokens _operator);
};

class HDLPrimaryAST : public HDLExpressionAST {
    bool identifierFlag = false;
    int base = 10;
    std::string identifier;
public:
    int width = 32;
    int value = 0;

    explicit HDLPrimaryAST(int constantNumber) :
            HDLExpressionAST(TOKEN_const_number),
            identifierFlag(false),
            value(constantNumber) {
        nodeType = "const_number";
    }

    explicit HDLPrimaryAST(int constantNumber,
                           int width, int base) :
            HDLExpressionAST(TOKEN_sized_number),
            identifierFlag(false),
            base(base),
            width(width),
            value(constantNumber) {
        nodeType = "const_number";
    }

    explicit HDLPrimaryAST(std::string identifier) :
            HDLExpressionAST(TOKEN_identifier),
            identifierFlag(true),
            identifier(std::move(identifier)) {
        nodeType = "identifier";
    }

    std::string toString() override;

    [[nodiscard]] bool isIdentifier() const;

    [[nodiscard]] std::string getIdentifier() const;
};

class HDLMuxAST : public HDLExpressionAST {
    static int muxCounter;
    std::string identifier;
public:

    explicit HDLMuxAST(std::unique_ptr<HDLExpressionAST> conditionExpr) :
            HDLExpressionAST(TOKEN_question),
            identifier("__hw_mux__" + std::to_string(muxCounter++)) {
        nodeType = "multiplexer";
        children.push_back(std::move(conditionExpr));
    }
};

class HDLConcatAST : public HDLExpressionAST {
    static int muxCounter;
    std::string identifier;
public:

    explicit HDLConcatAST() :
            HDLExpressionAST(TOKEN_lbrace),
            identifier("__hw_cat__" + std::to_string(muxCounter++)) {
        nodeType = "concatenation";
    }
};

class AlwaysBlockBodyAST : public AST {
    std::unique_ptr<HDLExpressionAST> condition;
public:
    explicit AlwaysBlockBodyAST(decltype(condition) cond) : AST("__hw_always_block_body__"),
                                                            condition(std::move(cond)) {}
};

class AlwaysBlockAST : public AST {
    std::vector<std::pair<TriggerEdgeType, std::string>> sensitiveList;
public:
    AlwaysBlockAST(const decltype(sensitiveList) &sensList, std::unique_ptr<AlwaysBlockBodyAST> bodyAST) :
            AST("__hw_always_block__"), sensitiveList(sensList) {
        auto *ptr = static_cast<AST *>(bodyAST.get());
        auto uniPtr = std::unique_ptr<AST>(ptr);
        children.push_back(std::move(uniPtr));
    }
};

#endif //VERIPYTHON_AST_H
