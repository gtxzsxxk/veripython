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

std::unordered_map<VeriPythonTokens, int> Parser::operatorPrecedence = {
        {TOKEN_op_add, 10},
        {TOKEN_op_sub, 10},
        {TOKEN_op_mul, 20},
        {TOKEN_op_div, 20},
};

std::unordered_map<VeriPythonTokens, std::string> Parser::operatorToString = {
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
    auto [_, identifierToken] = VERIFY_NEXT_TOKEN(identifier);
    hardwareModule.moduleName = identifierToken.second;
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
        hardwareModule.ioPorts.emplace_back(identifierToken.second);
    } else if (idOrInOut.first == TOKEN_input || idOrInOut.first == TOKEN_output) {
        nextToken();
        auto [lookAheadTokenReady, idOrPortSlicing] = lookAhead();
        if (!lookAheadTokenReady) {
            errorParsing("Unexpected EOF");
        }
        auto direction = idOrInOut.first == TOKEN_input ? PortDirection::Input : PortDirection::Output;
        if (idOrPortSlicing.first == TOKEN_identifier) {
            auto [_, identifierToken] = nextToken();
            hardwareModule.ioPorts.emplace_back(direction, identifierToken.second);
        } else if (idOrPortSlicing.first == TOKEN_lbracket) {
            auto *slicing = parsePortSlicing();
            auto [_, identifierToken] = nextToken();
            hardwareModule.ioPorts.emplace_back(direction, slicing, identifierToken.second);
        } else {
            errorParsing("Unexpected Token after input/output");
        }
    }
}

/*
 * portSlicing ::= "[" constantExpr "]"
 *             ||= "[" constantExpr ":" constantExpr "]"
 * */
PortSlicingAST *Parser::parsePortSlicing() {
    VERIFY_NEXT_TOKEN(lbracket);
    auto *firstValAST = parseConstantExpr();
    auto [lookAheadReady, lookAheadToken] = nextToken();
    if (!lookAheadReady) {
        errorParsing("Unexpected EOF");
    }

    PortSlicingAST *portSlicing;
    if (lookAheadToken.first == TOKEN_rbracket) {
        nextToken();
        portSlicing = new PortSlicingAST(firstValAST->eval());
    } else {
        VERIFY_NEXT_TOKEN(colon);
        auto *secondValAST = parseConstantExpr();
        VERIFY_NEXT_TOKEN(rbracket);
        portSlicing = new PortSlicingAST(firstValAST->eval(), secondValAST->eval());
        /* TODO: delete the two AST which are no longer used */
    }
    return portSlicing;
}

ConstantExpressionAST *Parser::parseConstantExpr() {
    /* a+b*c/2-1+(d+e)*c+2 */
    std::vector<ConstantExpressionAST *> astStack;
    std::vector<std::pair<VeriPythonTokens, int>> operatorStack;
    while (true) {
        int currentPrecedence = 0;
        if (!operatorStack.empty()) {
            currentPrecedence = operatorStack[operatorStack.size() - 1].second;
        }
        auto [tokenReady, lookAheadToken] = lookAhead();
        if (!tokenReady) {
            goto out;
        }
        if (lookAheadToken.first == TOKEN_const_number || lookAheadToken.first == TOKEN_lparen) {
            auto *ast = parseConstantPrimary();
            astStack.push_back(reinterpret_cast<ConstantExpressionAST *>(ast));

            /* 继续前瞻，根据后一个 operator 决定是否要对当前栈上元素进行合并 */
            auto [nextOpReady, nextOperator] = lookAhead();
            int nextPrecedence = getOperatorPrecedence(nextOperator);
            /* 如果下一个token不是operator，说明已经到达了结尾，直接当作 -1 处理就行 */
            /* 合并栈上元素，保证左结合性 */
            if (currentPrecedence >= nextPrecedence) {
                while (!operatorStack.empty()) {
                    VeriPythonTokens currentOperator = operatorStack[operatorStack.size() - 1].first;
                    auto *merge_ast = new ConstantExpressionAST(currentOperator);
                    merge_ast->children.push_back(astStack[astStack.size() - 2]);
                    merge_ast->children.push_back(astStack[astStack.size() - 1]);
                    astStack.pop_back();
                    astStack.pop_back();
                    operatorStack.pop_back();
                    astStack.push_back(reinterpret_cast<ConstantExpressionAST *>(merge_ast));

                    if (currentPrecedence == nextPrecedence) {
                        break;
                    }
                }
            }
        } else {
            int precedence = getOperatorPrecedence(lookAheadToken);
            if (precedence == -1) {
                goto out;
            }
            nextToken();
            operatorStack.emplace_back(lookAheadToken.first, precedence);
        }
    }

    out:
    if (!operatorStack.empty() || astStack.size() != 1) {
        errorParsing("Failed to parse constant expression");
    }
    return astStack[0];
}

/*
 * constantPrimary ::= const_number | "(" constantExpression ")"
 * */
ConstantExpressionAST *Parser::parseConstantPrimary() {
    auto [lookAheadReady, lookAheadTokenData] = lookAhead();
    if (!lookAheadReady) {
        errorParsing("Unexpected EOF");
    }
    decltype(parseConstantPrimary()) primaryAST;
    if (lookAheadTokenData.first == TOKEN_const_number) {
        nextToken();
        auto ast = new ConstantNumberAST(std::atoi(lookAheadTokenData.second.c_str()));
        primaryAST = ast;
    } else if (lookAheadTokenData.first == TOKEN_lparen) {
        nextToken();
        primaryAST = parseConstantExpr();
        VERIFY_NEXT_TOKEN(rparen);
    }
    return primaryAST;
}

/*
 * moduleBody ::= statement moduleBody | statement
 * */
void Parser::parseModuleBody() {
    auto [lookAheadReady, lookAheadTokenData] = lookAhead();
    if (!lookAheadReady) {
        errorParsing("Unexpected EOF");
    }
    if (lookAheadTokenData.first == TOKEN_input || lookAheadTokenData.first == TOKEN_output) {
        parseInputOutputStatement();
    } else if (lookAheadTokenData.first == TOKEN_assign) {
        parseAssignStatement();
    } else {
        errorParsing("Unexpected token");
    }
}

/*
 * output_stmt ::= "input"|"output" var1 "," var2 ";"
 * */
void Parser::parseInputOutputStatement() {
    auto [_, inOutToken] = nextToken();
    PortDirection direction;
    if (inOutToken.first == TOKEN_input) {
        direction = PortDirection::Input;
    } else if (inOutToken.first == TOKEN_output) {
        direction = PortDirection::Output;
    }
    while (true) {
        auto [_1, identifierToken] = VERIFY_NEXT_TOKEN(identifier);
        auto &port = hardwareModule.getModuleIOPortByName(identifierToken.second);
        port.setPortDirection(direction);
        auto [_2, commaOrSemicolonToken] = nextToken();
        if (commaOrSemicolonToken.first == TOKEN_semicolon) {
            return;
        } else if (commaOrSemicolonToken.first != TOKEN_comma) {
            errorParsing("Expecting comma");
        }
    }
}

/*
 * assign_stmt ::= "assign" id "=" HDLExpression ";"
 * */
void Parser::parseAssignStatement() {

}

void Parser::parseRegWireStatement() {

}

void Parser::parseHDLExpression() {

}

void Parser::parseHDLPrimary() {

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

int Parser::getOperatorPrecedence(LexTokenType &token) {
    if (token.first != TOKEN_op_add && token.first != TOKEN_op_sub &&
        token.first != TOKEN_op_mul && token.first != TOKEN_op_div) {
        return -1;
    }
    return operatorPrecedence[token.first];
}
