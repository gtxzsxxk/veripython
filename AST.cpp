//
// Created by hanyuan on 2024/11/2.
//
#include "AST.h"
#include "Parser.h"
#include <stdexcept>
#include <string>
#include <sstream>

std::string AST::toString() {
    std::stringstream out;
    out << "{\n"
        << R"(  "nodeType": ")" << nodeType << "\",\n"
        << "  \"children\": [\n";
    std::size_t counter = 0;
    for (const auto &ast: children) {
        std::string tmp = ast->toString();
        char *childOutput = strdup(tmp.c_str());
        for (char *line = strtok(childOutput, "\n"); line; line = strtok(nullptr, "\n")) {
            out << "    " << std::string{line} << "\n";
        }
        free(childOutput);
        if (counter != children.size() - 1) {
            out << "    ,\n";
        } else {
            out << "\n";
        }
        counter++;
    }
    out << "  ]\n"
        << "}";
    return out.str();
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
    return R"({\n  "nodeType": ")" + nodeType + "\",\n  \"value\": \"" + std::to_string(value) + "\"\n}";
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

std::string PortSlicingAST::toString() {
    if (isTrivial()) {
        return "{}";
    } else {
        if (isDownTo) {
            return "{\n  \"low\": \"" + std::to_string(downToLow) + "\",\n" +
                   "  \"high\": \"" + std::to_string(downToHigh) + "\"\n" +
                   "}";
        } else {
            return "{\n  \"onlyWhich\": \"" + std::to_string(onlyWhich) + "\"\n" +
                   "}";
        }
    }
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
    auto tmp = nodeType;
    nodeType = nodeName;
    auto ret = AST::toString();
    nodeType = tmp;
    return ret;
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
    return "{\n  \"nodeType\": \"" + nodeName + "\",\n  \"data\": \"" + output + "\"\n}";
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

std::shared_ptr<HDLExpressionAST> &AlwaysBlockBodyAST::getCondition() {
    return condition;
}

std::string AlwaysBlockBodyAST::toString() {
    if (nodeType == "__hw_always_block_body_if_block__") {
        std::string xmlOutput = "{\n  \"nodeType\": \"IfBlock\",\n";
        xmlOutput += "  \"condition\":\n";
        char *condOutput = strdup(condition->toString().c_str());
        for (char *line = strtok(condOutput, "\n"); line; line = strtok(nullptr, "\n")) {
            xmlOutput += "  " + std::string{line} + "\n";
        }
        free(condOutput);
        xmlOutput += "  ,\n";
        xmlOutput += "  \"branches\": [\n";
        std::size_t counter = 0;
        for (const auto &ast: children) {
            std::string tmp = ast->toString();
            char *childOutput = strdup(tmp.c_str());
            for (char *line = strtok(childOutput, "\n"); line; line = strtok(nullptr, "\n")) {
                xmlOutput += "    " + std::string{line} + "\n";
            }
            free(childOutput);
            if (counter != children.size() - 1) {
                xmlOutput += "    ,\n";
            } else {
                xmlOutput += "\n";
            }
            counter++;
        }
        xmlOutput += "  ]\n";
        xmlOutput += "}";
        return xmlOutput;
    } else if (nodeType == "__hw_non_blk_assign__") {
        std::string xmlOutput = "{\n  \"nodeType\": \"NonBlockAssign\",\n";
        xmlOutput += "  \"destinations\": [\n";
        auto *nonBlkAssignPtr = dynamic_cast<NonBlockingAssignAST *>(this);
        std::size_t counter = 0;
        auto &destData = nonBlkAssignPtr->connection.getDestIdentifiers();
        for (const auto &[name, slicing]: destData) {
            xmlOutput += "    {\n      \"nodeType\": \"CircuitSymbol\",\n";
            xmlOutput += "      \"name\": \"" + name + "\",\n";
            xmlOutput += "      \"slicing\":\n";
            auto slicingString = strdup(const_cast<PortSlicingAST &>(slicing).toString().c_str());
            for (char *line = strtok(slicingString, "\n"); line != nullptr; line = strtok(nullptr, "\n")) {
                xmlOutput += std::string{"        "} + line + "\n";
            }
            free(slicingString);
            if (counter != destData.size() - 1) {
                xmlOutput += "    },";
            } else {
                xmlOutput += "    }";
            }
            counter++;
        }
        xmlOutput += "  ],\n";

        xmlOutput += "  \"expression\": \n";
        auto exprString = strdup(nonBlkAssignPtr->connection.ast->toString().c_str());
        for (char *line = strtok(exprString, "\n"); line != nullptr; line = strtok(nullptr, "\n")) {
            xmlOutput += std::string{"    "} + line + "\n";
        }
        free(exprString);

        xmlOutput += "}";
        return xmlOutput;
    }
    return AST::toString();
}
