//
// Created by hanyuan on 2024/11/2.
//

#ifndef VERIPYTHON_AST_H
#define VERIPYTHON_AST_H
#include <string>
#include <utility>
#include <vector>

class AST {
public:
    std::string nodeType;
    std::vector<AST*> children;

    explicit AST(std::string nodeType): nodeType(std::move(nodeType)) {}
};

class NumberAST: public AST {
public:
    NumberAST() : AST("number") {}
    int value = -1;
};

#endif //VERIPYTHON_AST_H
