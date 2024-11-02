//
// Created by hanyuan on 2024/11/2.
//
#include "AST.h"

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
    return 0;
}

std::string NumberAST::toString() {
    return "<" + nodeType + ">\n    " + std::to_string(value) + "\n</" + nodeType + ">\n";
}
