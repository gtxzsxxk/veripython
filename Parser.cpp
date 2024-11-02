//
// Created by hanyuan on 2024/11/1.
//

#include "Parser.h"
#include <iostream>
#include <filesystem>
#include <cstdio>
#include <exception>

#define VERIFY_NEXT_TOKEN(name)         nextToken(TOKEN_##name, #name)

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

/*
 * module ::= "module" identifier "(" module_port_list ")" ";" module_body "endmodule"
 * */
void Parser::parseModule() {
    VERIFY_NEXT_TOKEN(module);
    auto identifierTokenData = VERIFY_NEXT_TOKEN(identifier);
    VERIFY_NEXT_TOKEN(lparen);
    parseModulePortList();
    VERIFY_NEXT_TOKEN(rparen);
    VERIFY_NEXT_TOKEN(semicolon);
    parseModuleBody();
    VERIFY_NEXT_TOKEN(endmodule);
}

/*
 * modulePortList ::= modulePort "," modulePortList | modulePort
 * */
void Parser::parseModulePortList() {
    auto [tokenReady, rparenOrId] = lookAhead();
    if (!tokenReady) {
        errorParsing("Unexpected EOF");
    }
    parseModulePort();
    auto [commaTokenReady, commaOrRparen] = lookAhead();
    if (!commaTokenReady) {
        errorParsing("Unexpected EOF");
    }
    if (commaOrRparen.first == TOKEN_comma) {
        nextToken();
        parseModulePortList();
    } else if (commaOrRparen.first == TOKEN_rparen) {
        return;
    } else {
        errorParsing("Unexpected Token after parseModulePort");
    }
}

/*
 * modulePort ::= id
 *            ||= "input"|"output" portSlicing id
 *            ||= "input"|"output" id
 * */
void Parser::parseModulePort() {
    auto [tokenReady, idOrInOut] = lookAhead();
    if(!tokenReady) {
        errorParsing("Unexpected EOF");
    }
    if(idOrInOut.first == TOKEN_identifier) {
        auto [_, identifierToken] = nextToken();
    } else if(idOrInOut.first == TOKEN_input || idOrInOut.first == TOKEN_output) {
        nextToken();
        auto [lookAheadTokenReady, idOrPortSlicing] = lookAhead();
        if(!lookAheadTokenReady) {
            errorParsing("Unexpected EOF");
        }
        if(idOrPortSlicing.first == TOKEN_identifier) {
            auto [_, identifierToken] = nextToken();
        } else if(idOrPortSlicing.first == TOKEN_lbracket) {
            parsePortSlicing();
            auto [_, identifierToken] = nextToken();
        } else {
            errorParsing("Unexpected Token after input/output");
        }
    }
}

/*
 * portSlicing ::= "[" constantExpr "]"
 *             ||= "[" constantExpr ":" constantExpr "]"
 * */
void Parser::parsePortSlicing() {

}

void Parser::parseModuleBody() {

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

std::tuple<bool, LexTokenType>
Parser::nextToken(enum VeriPythonTokens expectedTokenEnum, const std::string &expectTokenName) {
    auto tokenTuple = nextToken();
    if (std::get<0>(tokenTuple) && std::get<1>(tokenTuple).first != expectedTokenEnum) {
        errorParsing("Unexpected token", expectTokenName);
    }
    return tokenTuple;
}

void Parser::errorParsing(const std::string &message, const std::string &expectTokenName) {
    std::cerr << "Verilog parsing error: " << message << "\n";
    std::cerr << "Expecting token: " << expectTokenName << "\n";
    std::cerr << "Fetched Tokens: " << "\n";
    while (!tokenBuffer.empty()) {
        auto tokenData = tokenBuffer.front();
        tokenBuffer.pop();
        std::cerr << tokenData.second << " ";
    }
    std::cerr << std::endl;
    throw std::runtime_error("Code syntax error");
}
