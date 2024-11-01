//
// Created by hanyuan on 2024/11/1.
//

#include "Parser.h"
#include <iostream>
#include <filesystem>
#include <cstdio>

extern "C" {
int yylex();
extern char *yytext;
extern FILE *yyin;
}

union LEXER_VALUE yylval;

Parser::Parser(const std::string &filename) {
    yyin = fopen(filename.c_str(), "r");
}

/*
 * hdl ::= module hdl | epsilon
 * */
void Parser::parseHDL() {
    auto [tokenReady, tokenData] = lookAhead();
    if (!tokenReady) {
        return;
    }
    if (tokenData.first == TOKEN_module) {
        parseModule();
        parseHDL();
    } else {
        errorParsing("Unexpected lookahead at parseHDL", "module");
    }
}

void Parser::parseModule() {

}

Parser::~Parser() {
    fclose(yyin);
}

std::tuple<bool, LexTokenType> Parser::lookAhead() {
    if (tokenBuffer.empty()) {
        auto [tokenRdy, tokenData] = nextToken();
        if (tokenRdy) {
            tokenBuffer.emplace(tokenData);
            return {tokenRdy, tokenData};
        }
        return {tokenRdy, tokenData};
    } else {
        auto tokenData = tokenBuffer.front();
        return {true, tokenData};
    }
}

std::tuple<bool, LexTokenType> Parser::nextToken() {
    if (tokenBuffer.empty()) {
        int ret = yylex();
        if (ret == 0) {
            return {false, {TOKEN_assign, ""}};
        }
        std::string data{yylval.str};
        free(yylval.str);
        LexTokenType tokenData = {VeriPythonTokens(ret), data};
        tokenFetched.push(tokenData);
        if (tokenFetched.size() > TOKEN_FETCHED_SIZE) {
            tokenFetched.pop();
        }
        return {true, tokenData};
    } else {
        auto tokenData = tokenBuffer.front();
        tokenBuffer.pop();
        return {true, tokenData};
    }
}

void Parser::errorParsing(const std::string &message, const std::string &expectToken) {
std::tuple<bool, LexTokenType>
Parser::nextToken(enum VeriPythonTokens expectedTokenEnum, const std::string &expectTokenName) {
    auto tokenTuple = nextToken();
    if (std::get<0>(tokenTuple) && std::get<1>(tokenTuple).first != expectedTokenEnum) {
        errorParsing("Unexpected token", expectTokenName);
    }
    return tokenTuple;
}

    std::cerr << "Verilog parsing error: " << message << "\n";
    std::cerr << "Expecting token: " << expectToken << "\n";
    std::cerr << "Fetched Tokens: " << "\n";
    while (!tokenBuffer.empty()) {
        auto tokenData = tokenBuffer.front();
        tokenBuffer.pop();
        std::cerr << tokenData.second << " ";
    }
    std::cerr << std::endl;
}
