//
// Created by unknown on 2024/11/1.
//

#ifndef VERIPYTHON_PARSER_H
#define VERIPYTHON_PARSER_H

#include "Lexer.h"
#include "AST.h"
#include "RtlModule.h"
#include <memory>
#include <string>
#include <tuple>
#include <queue>
#include <unordered_map>

class Parser {
    using LexTokenType = std::tuple<enum VeriPythonTokens, std::string, int, int>;

    const std::size_t TOKEN_FETCHED_SIZE = 100;
    std::queue<LexTokenType> tokenBuffer;
    std::string sourceFileName;
    std::vector<std::string> sourceFileLines;

    static int getConstantOperatorPrecedence(LexTokenType &token);

    static int getHDLOperatorPrecedence(LexTokenType &token);

    std::tuple<bool, LexTokenType> lookAhead();

    std::tuple<bool, LexTokenType> nextToken();

    std::tuple<bool, LexTokenType> nextToken(enum VeriPythonTokens expectedTokenEnum,
                                             const std::string &expectTokenName);

    void printErrorToken(std::ostream &out, const LexTokenType &errorToken, const std::string &msg);

    void errorParsing(const LexTokenType &errorToken,
                      const std::string &message,
                      const std::string &expectTokenName = "");

    void warningParsing(const LexTokenType &errorToken,
                        const std::string &message);

    void parseModule();

    void parseModulePortList();

    void parseModuleBody();

    void parseModulePort();

    void parseInputOutputStatement();

    void parseCombAssignStatement();

    void parseRegWireStatement();

    std::unique_ptr<AlwaysBlockAST> parseAlwaysBlock();

    std::unique_ptr<AlwaysBlockBodyAST> parseAlwaysBlockBody();

    std::unique_ptr<AlwaysBlockBodyAST> parseAlwaysBlockBodyStatement();

    std::unique_ptr<AlwaysBlockBodyAST> parseIfBlock();

    CircuitConnection parseNonBlockingAssignment();

    std::vector<std::pair<TriggerEdgeType, std::string>> parseSensitiveList();

    std::unique_ptr<HDLExpressionAST> parseHDLPrimary();

    PortSlicingAST parsePortSlicing();

    std::unique_ptr<ConstantExpressionAST> parseConstantPrimary();

public:
    RtlModule hardwareModule;
    static std::unordered_map<VeriPythonTokens, int> operatorPrecedence;
    static std::unordered_map<VeriPythonTokens, std::string> operatorName;

    explicit Parser(const std::string &filename);

    ~Parser();

    void parseHDL();

    std::unique_ptr<ConstantExpressionAST> parseConstantExpr();

    std::unique_ptr<HDLExpressionAST> parseHDLExpression();
};

#endif //VERIPYTHON_PARSER_H
