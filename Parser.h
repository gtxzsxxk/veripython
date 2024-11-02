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
    const int TOKEN_FETCHED_SIZE = 100;
    std::queue<LexTokenType> tokenBuffer;
    std::queue<LexTokenType> tokenFetched;

    std::tuple<bool, LexTokenType> lookAhead();

    std::tuple<bool, LexTokenType> nextToken();

    std::tuple<bool, LexTokenType> nextToken(enum VeriPythonTokens expectedTokenEnum,
                                             const std::string &expectTokenName);

    void errorParsing(const std::string &message, const std::string &expectTokenName = "");

    void parseModule();

    void parseModulePortList();

    void parseModuleBody();

    void parseModulePort();

public:
    explicit Parser(const std::string &filename);

    ~Parser();

    void parseHDL();
};

#endif //VERIPYTHON_PARSER_H
