//
// Created by hanyuan on 2024/11/1.
//

#ifndef VERIPYTHON_PARSER_H
#define VERIPYTHON_PARSER_H

#include "Lexer.h"
#include "AST.h"
#include "HardwareModel.h"
#include <string>
#include <tuple>
#include <queue>
#include <unordered_map>

typedef std::pair<enum VeriPythonTokens, std::string> LexTokenType;

class Parser {
    const std::size_t TOKEN_FETCHED_SIZE = 100;
    std::queue<LexTokenType> tokenBuffer;
    std::queue<LexTokenType> tokenFetched;

    static int getOperatorPrecedence(LexTokenType &token);

    std::tuple<bool, LexTokenType> lookAhead();

    std::tuple<bool, LexTokenType> nextToken();

    std::tuple<bool, LexTokenType> nextToken(enum VeriPythonTokens expectedTokenEnum,
                                             const std::string &expectTokenName);

    void errorParsing(const std::string &message, const std::string &expectTokenName = "");

    void parseModule();

    void parseModulePortList();

    void parseModuleBody();

    void parseModulePort();

    void parseInputOutputStatement();

    void parseAssignStatement();

    void parseRegWireStatement();

    HDLExpressionAST *parseHDLPrimary();

    PortSlicingAST *parsePortSlicing();

    ConstantExpressionAST *parseConstantPrimary();

public:
    HardwareModule hardwareModule;
    static std::unordered_map<VeriPythonTokens, int> operatorPrecedence;
    static std::unordered_map<VeriPythonTokens, std::string> operatorName;

    explicit Parser(const std::string &filename);

    ~Parser();

    void parseHDL();

    ConstantExpressionAST *parseConstantExpr();

    HDLExpressionAST *parseHDLExpression();
};

#endif //VERIPYTHON_PARSER_H
