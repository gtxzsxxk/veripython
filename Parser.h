//
// Created by hanyuan on 2024/11/1.
//

#ifndef VERIPYTHON_PARSER_H
#define VERIPYTHON_PARSER_H

#include "Lexer.h"
#include <string>
#include <tuple>
#include <queue>

typedef std::pair<enum VeriPythonTokens, std::string> LexTokenType;

class Parser {
    std::tuple<bool, enum VeriPythonTokens> nextToken();
    std::queue<LexTokenType> tokenBuffer;

    std::tuple<bool, LexTokenType> lookAhead();

    std::tuple<bool, LexTokenType> nextToken();

public:
    explicit Parser(const std::string &filename);

    ~Parser();

    void doParse();
};

#endif //VERIPYTHON_PARSER_H
