//
// Created by hanyuan on 2024/11/1.
//

#ifndef VERIPYTHON_PARSER_H
#define VERIPYTHON_PARSER_H

#include "Lexer.h"
#include <string>
#include <tuple>

class Parser {
    std::tuple<bool, enum VeriPythonTokens> nextToken();

public:
    explicit Parser(const std::string &filename);

    ~Parser();

    void doParse();
};

#endif //VERIPYTHON_PARSER_H
