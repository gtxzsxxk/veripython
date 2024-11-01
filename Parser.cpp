//
// Created by hanyuan on 2024/11/1.
//

#include "Parser.h"
#include <iostream>
#include <filesystem>

extern "C" {
    int yylex();
    extern char* yytext;
}

union LEXER_VALUE yylval;

void Parser::doParse() {
    auto tok = yylex();
    std::cout << std::filesystem::current_path() << std::endl;
}
