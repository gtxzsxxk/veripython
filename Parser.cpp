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

void Parser::doParse() {

}

Parser::~Parser() {
    fclose(yyin);
}

std::tuple<bool, enum VeriPythonTokens> Parser::nextToken() {
    int ret = yylex();
    if (ret == 0) {
        return {false, TOKEN_assign};
    }
    return {true, VeriPythonTokens(ret)};
}
