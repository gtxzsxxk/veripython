//
// Created by hanyuan on 2024/11/1.
//

#include "Parser.h"
#include <iostream>
#include <filesystem>
#include <cstdio>
#include <exception>
#include <vector>

#define VERIFY_NEXT_TOKEN(name)         nextToken(TOKEN_##name, #name)

extern "C" {
int yylex();
extern char *yytext;
extern FILE *yyin;
}

union LEXER_VALUE yylval;

std::unordered_map<VeriPythonTokens, int> Parser::binaryOpPrecedence = {
        {TOKEN_op_add, 10},
        {TOKEN_op_sub, 10},
        {TOKEN_op_mul, 20},
        {TOKEN_op_div, 20},
};

std::unordered_map<VeriPythonTokens, std::string> Parser::binaryOpToString = {
        {TOKEN_op_add, "op_add"},
        {TOKEN_op_sub, "op_sub"},
        {TOKEN_op_mul, "op_mul"},
        {TOKEN_op_div, "op_div"},
};

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
    if (!tokenReady) {
        errorParsing("Unexpected EOF");
    }
    if (idOrInOut.first == TOKEN_identifier) {
        auto [_, identifierToken] = nextToken();
    } else if (idOrInOut.first == TOKEN_input || idOrInOut.first == TOKEN_output) {
        nextToken();
        auto [lookAheadTokenReady, idOrPortSlicing] = lookAhead();
        if (!lookAheadTokenReady) {
            errorParsing("Unexpected EOF");
        }
        if (idOrPortSlicing.first == TOKEN_identifier) {
            auto [_, identifierToken] = nextToken();
        } else if (idOrPortSlicing.first == TOKEN_lbracket) {
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

ConstantExpressionAST *Parser::parseConstantExpr() {
    /* a+b*c/2-1+(d+e)*c+2 */
    std::vector<ConstantExpressionAST *> astStack;
    std::vector<std::pair<VeriPythonTokens, int>> operandStack;
    while (true) {
        int currentPrecedence = 0;
        VeriPythonTokens currentOperand = TOKEN_identifier;
        if (!operandStack.empty()) {
            currentPrecedence = operandStack[operandStack.size() - 1].second;
            currentOperand = operandStack[operandStack.size() - 1].first;
        }
        auto [tokenReady, lookAheadToken] = lookAhead();
        if (!tokenReady) {
            goto out;
        }
        if (lookAheadToken.first == TOKEN_number) {
            nextToken();
            auto *ast = new NumberAST(std::atoi(lookAheadToken.second.c_str()));
            astStack.push_back(reinterpret_cast<ConstantExpressionAST *>(ast));

            /* 继续前瞻，根据后一个 operand 决定是否要对当前栈上元素进行合并 */
            auto [nextOpReady, nextOperand] = lookAhead();
            int nextPrecedence = getOperandPrecedence(nextOperand);
            /* 如果下一个token不是operand，说明已经到达了结尾，直接当作 -1 处理就行 */
            if (currentPrecedence >= nextPrecedence) {
                /* 合并栈上元素 */
                auto *merge_ast = new AST(binaryOpToString[currentOperand]);
                merge_ast->children.push_back(astStack[astStack.size() - 2]);
                merge_ast->children.push_back(astStack[astStack.size() - 1]);
                astStack.pop_back();
                astStack.pop_back();
                operandStack.pop_back();
                astStack.push_back(reinterpret_cast<ConstantExpressionAST *>(merge_ast));
            }
        } else {
            int precedence = getOperandPrecedence(lookAheadToken);
            if (precedence == -1) {
                goto out;
            }
            nextToken();
            operandStack.emplace_back(lookAheadToken.first, precedence);
        }
    }

    out:
    while (!operandStack.empty()) {
        VeriPythonTokens currentOperand = operandStack[operandStack.size() - 1].first;
        auto *merge_ast = new AST(binaryOpToString[currentOperand]);
        merge_ast->children.push_back(astStack[astStack.size() - 2]);
        merge_ast->children.push_back(astStack[astStack.size() - 1]);
        astStack.pop_back();
        astStack.pop_back();
        operandStack.pop_back();
        astStack.push_back(reinterpret_cast<ConstantExpressionAST *>(merge_ast));
    }
    return astStack[0];
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

int Parser::getOperandPrecedence(LexTokenType &token) {
    if (token.first != TOKEN_op_add && token.first != TOKEN_op_sub &&
        token.first != TOKEN_op_mul && token.first != TOKEN_op_div) {
        return -1;
    }
    return binaryOpPrecedence[token.first];
}
