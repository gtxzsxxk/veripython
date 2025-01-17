//
// Created by unknown on 2024/11/2.
//

#ifndef VERIPYTHON_AST_H
#define VERIPYTHON_AST_H

#include "Lexer.h"
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include <cstring>

class HDLExpressionAST;

enum class PortDirection {
    Input,
    Output,
    Unspecified
};

enum class TriggerEdgeType {
    NOT_SPECIFIED,
    POSITIVE_EDGE,
    NEGATIVE_EDGE
};

class AST {
public:
    std::string nodeType;
    std::vector<std::unique_ptr<AST>> children;
    std::shared_ptr<HDLExpressionAST> condition = nullptr;

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

    bool operator==(const PortSlicingAST &slicingAST) const;

    [[nodiscard]] bool isTrivial() const;

    std::string toString() override;

    [[nodiscard]] int32_t getWidth() const;

    [[nodiscard]] std::pair<int, int> toDownTo() const;
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
public:
    std::string identifier;

    explicit HDLMuxAST(bool isFake = false) :
            HDLExpressionAST(TOKEN_question),
            identifier((isFake ? "__hw_mux_fake__" : "__hw_mux__") + std::to_string(muxCounter++)) {
        nodeType = "multiplexer";
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
public:
    explicit AlwaysBlockBodyAST(std::shared_ptr<HDLExpressionAST> cond) : AST("__hw_always_block_body__") {
        condition = std::move(cond);
    }

    std::shared_ptr<HDLExpressionAST> &getCondition();

    std::string toString() override;
};

class AlwaysBlockAST : public AST {
    std::vector<std::pair<TriggerEdgeType, std::string>> sensitiveList;
public:
    AlwaysBlockAST(decltype(sensitiveList) sensList, std::unique_ptr<AlwaysBlockBodyAST> &&bodyAST) :
            AST("__hw_always_block__"), sensitiveList(std::move(sensList)) {
        auto *ptr = static_cast<AST *>(bodyAST.release());
        auto uniPtr = std::unique_ptr<AST>(ptr);
        children.push_back(std::move(uniPtr));
    }

    [[nodiscard]] const decltype(sensitiveList) &getSensitiveList() const;
};

#endif //VERIPYTHON_AST_H
