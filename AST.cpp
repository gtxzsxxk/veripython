//
// Created by hanyuan on 2024/11/2.
//
#include "AST.h"
#include "Parser.h"
#include <stdexcept>
#include <string>

std::string AST::toString() {
    std::string xmlOutput = "<" + nodeType + ">\n";
    for (const auto &ast: children) {
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
    int lhsValue = dynamic_cast<ConstantExpressionAST *>(children[0].get())->eval();
    int rhsValue = dynamic_cast<ConstantExpressionAST *>(children[1].get())->eval();
    int answer;
    switch (_operator) {
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
            throw std::runtime_error("Unsupported operator");
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

PortSlicingAST::PortSlicingAST(const PortSlicingAST &slicingAST) : AST("PortSlicing") {
    isDownTo = slicingAST.isDownTo;
    downToHigh = slicingAST.downToHigh;
    downToLow = slicingAST.downToLow;
    onlyWhich = slicingAST.onlyWhich;
}

PortSlicingAST &PortSlicingAST::operator=(const PortSlicingAST &slicingAST) {
    isDownTo = slicingAST.isDownTo;
    downToHigh = slicingAST.downToHigh;
    downToLow = slicingAST.downToLow;
    onlyWhich = slicingAST.onlyWhich;
    return *this;
}

bool PortSlicingAST::isTrivial() const {
    return isDownTo && downToHigh == -1 && downToLow == -1;
}

bool PortSlicingAST::operator==(const PortSlicingAST &slicingAST) const {
    return this->isDownTo == slicingAST.isDownTo &&
           ((this->isDownTo && this->downToHigh == slicingAST.downToHigh && this->downToLow == slicingAST.downToLow) ||
            (!this->isDownTo && this->onlyWhich == slicingAST.onlyWhich)
           );
}

void HDLExpressionAST::setExprSlicing(const PortSlicingAST &slicingAst) {
    exprSlicing = slicingAst;
}

std::string HDLExpressionAST::toString() {
    std::string nodeName = nodeType;
    if (Parser::operatorName.count(_operator) > 0) {
        nodeName = Parser::operatorName[_operator];
    }
    if (exprSlicing.isDownTo) {
        nodeName += "__" + std::to_string(exprSlicing.downToHigh) + "_" + std::to_string(exprSlicing.downToLow);
    } else {
        nodeName += "__" + std::to_string(exprSlicing.onlyWhich);
    }
    std::string xmlOutput = "<" + nodeName + ">\n";
    for (const auto &ast: children) {
        std::string tmp = ast->toString();
        char *childOutput = strdup(tmp.c_str());
        for (char *line = strtok(childOutput, "\n"); line; line = strtok(nullptr, "\n")) {
            xmlOutput += "    " + std::string{line} + "\n";
        }
        free(childOutput);
    }
    xmlOutput += "</" + nodeName + ">\n";
    return xmlOutput;
}

bool HDLExpressionAST::canParseToCombLogics(VeriPythonTokens _operator) {
    return _operator == TOKEN_lbrace || Parser::operatorPrecedence.count(_operator) > 0;
}

std::string HDLPrimaryAST::toString() {
    std::string output;
    std::string nodeName = nodeType;
    if (identifierFlag) {
        output = identifier;
    } else {
        if (base == 10) {
            output = std::to_string(width) + "'d" + std::to_string(value);
        } else {
            char buffer[32];
            std::sprintf(buffer, "%x", value);
            output = std::to_string(width) + "'h" + std::string{buffer};
        }
    }
    if (exprSlicing.isDownTo) {
        nodeName += "__" + std::to_string(exprSlicing.downToHigh) + "_" + std::to_string(exprSlicing.downToLow);
    } else {
        nodeName += "__" + std::to_string(exprSlicing.onlyWhich);
    }
    return "<" + nodeName + ">\n    " + output + "\n</" + nodeName + ">\n";
}

bool HDLPrimaryAST::isIdentifier() const {
    return identifierFlag;
}

std::string HDLPrimaryAST::getIdentifier() const {
    return identifier;
}

int HDLMuxAST::muxCounter = 0;
int HDLConcatAST::muxCounter = 0;

const decltype(AlwaysBlockAST::sensitiveList) &AlwaysBlockAST::getSensitiveList() const {
    return sensitiveList;
}

std::unique_ptr<HDLExpressionAST> &&AlwaysBlockBodyAST::getCondition() {
    return std::move(condition);
}
