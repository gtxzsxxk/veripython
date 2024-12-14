//
// Created by hanyuan on 2024/11/1.
//

#include "Parser.h"
#include <iostream>
#include <fstream>
#include <cstdio>
#include <exception>
#include <vector>
#include <algorithm>

#define VERIFY_NEXT_TOKEN(name)         nextToken(TOKEN_##name, #name)

extern "C" {
int yylex();
extern char *yytext;
extern FILE *yyin;
extern int yy_lineno;
extern int yycolumn;
}

union LEXER_VALUE yylval;

std::unordered_map<VeriPythonTokens, int> Parser::operatorPrecedence = {
        {TOKEN_colon,          5},
        {TOKEN_question,       5},
        {TOKEN_logical_and,    20},
        {TOKEN_bitwise_or,     30},
        {TOKEN_bitwise_xor,    40},
        {TOKEN_bitwise_and,    50},
        {TOKEN_cond_eq,        60},
        {TOKEN_cond_ne,        60},
        {TOKEN_cond_lt,        70},
        {TOKEN_cond_le,        70},
        {TOKEN_cond_gt,        70},
        {TOKEN_cond_ge,        70},
        {TOKEN_arith_lshift,   80},
        {TOKEN_logical_lshift, 80},
        {TOKEN_arith_rshift,   80},
        {TOKEN_logical_rshift, 80},
        {TOKEN_op_add,         90},
        {TOKEN_op_sub,         90},
        {TOKEN_op_mod,         100},
        {TOKEN_op_mul,         100},
        {TOKEN_op_div,         100},
        {TOKEN_logical_not,    110},
        {TOKEN_bitwise_not,    110},
};

std::unordered_map<VeriPythonTokens, std::string> Parser::operatorName = {
        {TOKEN_lbrace,         "concat"},
        {TOKEN_question,       "multiplexer"},
        {TOKEN_logical_or,     "logical_or"},
        {TOKEN_logical_and,    "logical_and"},
        {TOKEN_bitwise_or,     "bitwise_or"},
        {TOKEN_bitwise_xor,    "bitwise_xor"},
        {TOKEN_bitwise_and,    "bitwise_and"},
        {TOKEN_cond_eq,        "cond_eq"},
        {TOKEN_cond_ne,        "cond_ne"},
        {TOKEN_cond_lt,        "cond_lt"},
        {TOKEN_cond_le,        "cond_le"},
        {TOKEN_cond_gt,        "cond_gt"},
        {TOKEN_cond_ge,        "cond_ge"},
        {TOKEN_arith_lshift,   "arith_lshift"},
        {TOKEN_logical_lshift, "logical_lshift"},
        {TOKEN_arith_rshift,   "arith_rshift"},
        {TOKEN_logical_rshift, "logical_rshift"},
        {TOKEN_op_add,         "op_add"},
        {TOKEN_op_sub,         "op_sub"},
        {TOKEN_op_mod,         "op_mod"},
        {TOKEN_op_mul,         "op_mul"},
        {TOKEN_op_div,         "op_div"},
        {TOKEN_logical_not,    "logical_not"},
        {TOKEN_bitwise_not,    "bitwise_not"}
};

Parser::Parser(const std::string &filename) : sourceFileName(filename) {
    std::ifstream src(filename);
    std::string line;
    while (std::getline(src, line)) {
        sourceFileLines.push_back(line);
    }
    src.close();
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
    if (std::get<0>(tokenData) == TOKEN_module) {
        parseModule();
        parseHDL();
    } else {
        errorParsing(tokenData, "Unexpected lookahead at parseHDL", "module");
    }
}

/*
 * module ::= "module" identifier "(" module_port_list ")" ";" module_body "endmodule"
 * */
void Parser::parseModule() {
    VERIFY_NEXT_TOKEN(module);
    auto [_, identifierToken] = VERIFY_NEXT_TOKEN(identifier);
    hardwareModule.moduleName = std::get<1>(identifierToken);
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
        std::get<2>(rparenOrId) = -1;
        std::get<3>(rparenOrId) = -1;
        errorParsing(rparenOrId, "Unexpected EOF");
    }
    parseModulePort();
    auto [commaTokenReady, commaOrRparen] = lookAhead();
    if (!commaTokenReady) {
        std::get<2>(commaOrRparen) = -1;
        std::get<3>(commaOrRparen) = -1;
        errorParsing(commaOrRparen, "Unexpected EOF");
    }
    if (std::get<0>(commaOrRparen) == TOKEN_comma) {
        nextToken();
        parseModulePortList();
    } else if (std::get<0>(commaOrRparen) == TOKEN_rparen) {
        return;
    } else {
        errorParsing(commaOrRparen, "Unexpected Token after parseModulePort");
    }
}

/*
 * modulePort ::= id
 *            ||= "input"|"output" ("wire"|"reg")? portSlicing id
 *            ||= "input"|"output" ("wire"|"reg") id
 * */
void Parser::parseModulePort() {
    auto [tokenReady, idOrInOut] = lookAhead();
    if (!tokenReady) {
        std::get<2>(idOrInOut) = -1;
        std::get<3>(idOrInOut) = -1;
        errorParsing(idOrInOut, "Unexpected EOF");
    }
    auto idOrInOutTkEnum = std::get<0>(idOrInOut);
    if (idOrInOutTkEnum == TOKEN_identifier) {
        auto [_, identifierToken] = nextToken();
        hardwareModule.ioPorts.push_back(std::make_shared<ModuleIOPort>(std::get<1>(identifierToken)));
    } else if (idOrInOutTkEnum == TOKEN_input || idOrInOutTkEnum == TOKEN_output) {
        nextToken();
        auto [_, wireOrRegOrOther] = lookAhead();
        decltype(wireOrRegOrOther) idOrPortSlicing;
        auto wireOrRegOrOtherTkEnum = std::get<0>(wireOrRegOrOther);
        if (wireOrRegOrOtherTkEnum == TOKEN_wire) {
            nextToken();
            idOrPortSlicing = std::get<1>(lookAhead());
        } else if (wireOrRegOrOtherTkEnum == TOKEN_reg) {
            nextToken();
            idOrPortSlicing = std::get<1>(lookAhead());
            warningParsing(wireOrRegOrOther, "Marking a port as register is not supported by this tool "
                                             "and will finally built as a wire. Keep it if you make sure "
                                             "the related circuit is combinatorial.");
        } else {
            idOrPortSlicing = wireOrRegOrOther;
        }
        auto direction = idOrInOutTkEnum == TOKEN_input ? PortDirection::Input : PortDirection::Output;
        auto idOrPortSlicingTkEnum = std::get<0>(idOrPortSlicing);
        if (idOrPortSlicingTkEnum == TOKEN_identifier) {
            auto [_, identifierToken] = nextToken();
            hardwareModule.ioPorts.push_back(std::make_shared<ModuleIOPort>(direction, std::get<1>(identifierToken)));
        } else if (idOrPortSlicingTkEnum == TOKEN_lbracket) {
            auto slicing = parsePortSlicing();
            auto [_, identifierToken] = nextToken();
            hardwareModule.ioPorts.push_back(
                    std::make_shared<ModuleIOPort>(direction, slicing, std::get<1>(identifierToken)));
        } else {
            errorParsing(idOrPortSlicing, "Unexpected Token after input/output");
        }
    }
}

/*
 * portSlicing ::= "[" constantExpr "]"
 *             ||= "[" constantExpr ":" constantExpr "]"
 * */
PortSlicingAST Parser::parsePortSlicing() {
    VERIFY_NEXT_TOKEN(lbracket);
    auto firstValAST = parseConstantExpr();
    auto [lookAheadReady, lookAheadToken] = nextToken();
    if (!lookAheadReady) {
        std::get<2>(lookAheadToken) = -1;
        std::get<3>(lookAheadToken) = -1;
        errorParsing(lookAheadToken, "Unexpected EOF");
    }

    PortSlicingAST portSlicing{0};
    if (std::get<0>(lookAheadToken) == TOKEN_rbracket) {
        portSlicing = PortSlicingAST{firstValAST->eval()};
    } else {
        auto secondValAST = parseConstantExpr();
        VERIFY_NEXT_TOKEN(rbracket);
        portSlicing = PortSlicingAST{firstValAST->eval(), secondValAST->eval()};
    }

    return portSlicing;
}

std::unique_ptr<ConstantExpressionAST> Parser::parseConstantExpr() {
    /* a+b*c/2-1+(d+e)*c+2 */
    std::vector<std::unique_ptr<ConstantExpressionAST>> astStack;
    std::vector<std::pair<VeriPythonTokens, int>> operatorStack;
    LexTokenType outToken;
    while (true) {
        int currentPrecedence = 0;
        if (!operatorStack.empty()) {
            currentPrecedence = operatorStack[operatorStack.size() - 1].second;
        }
        auto [tokenReady, lookAheadToken] = lookAhead();
        if (!tokenReady) {
            outToken = lookAheadToken;
            std::get<2>(outToken) = -1;
            std::get<3>(outToken) = -1;
            break;
        }
        if (std::get<0>(lookAheadToken) == TOKEN_const_number || std::get<0>(lookAheadToken) == TOKEN_lparen) {
            auto ast = parseConstantPrimary();
            astStack.push_back(std::move(ast));

            /* 继续前瞻，根据后一个 operator 决定是否要对当前栈上元素进行合并 */
            auto [nextOpReady, nextOperator] = lookAhead();
            int nextPrecedence = getConstantOperatorPrecedence(nextOperator);
            /* 如果下一个token不是operator，说明已经到达了结尾，直接当作 -1 处理就行 */
            /* 合并栈上元素，保证左结合性 */
            if (currentPrecedence >= nextPrecedence) {
                while (!operatorStack.empty()) {
                    VeriPythonTokens currentOperator = operatorStack[operatorStack.size() - 1].first;
                    auto merge_ast = std::make_unique<ConstantExpressionAST>(currentOperator);
                    merge_ast->children.push_back(std::move(astStack[astStack.size() - 2]));
                    merge_ast->children.push_back(std::move(astStack[astStack.size() - 1]));
                    astStack.pop_back();
                    astStack.pop_back();
                    operatorStack.pop_back();
                    astStack.push_back(std::move(merge_ast));

                    if (currentPrecedence == nextPrecedence) {
                        outToken = nextOperator;
                        break;
                    }
                }
            }
        } else {
            int precedence = getConstantOperatorPrecedence(lookAheadToken);
            if (precedence == -1) {
                outToken = lookAheadToken;
                break;
            }
            nextToken();
            operatorStack.emplace_back(std::get<0>(lookAheadToken), precedence);
        }
    }

    if (!operatorStack.empty() || astStack.size() != 1) {
        errorParsing(outToken, "Failed to parse constant expression");
    }
    return std::move(astStack[0]);
}

/*
 * constantPrimary ::= const_number | "(" constantExpression ")"
 * */
std::unique_ptr<ConstantExpressionAST> Parser::parseConstantPrimary() {
    auto [lookAheadReady, lookAheadTokenData] = lookAhead();
    if (!lookAheadReady) {
        std::get<2>(lookAheadTokenData) = -1;
        std::get<3>(lookAheadTokenData) = -1;
        errorParsing(lookAheadTokenData, "Unexpected EOF");
    }
    decltype(parseConstantPrimary()) primaryAST;
    if (std::get<0>(lookAheadTokenData) == TOKEN_const_number) {
        nextToken();
        auto ast = static_cast<ConstantExpressionAST *>(new ConstantNumberAST{
                std::stoi(std::get<1>(lookAheadTokenData))});
        primaryAST = std::unique_ptr<ConstantExpressionAST>{ast};
    } else if (std::get<0>(lookAheadTokenData) == TOKEN_lparen) {
        nextToken();
        primaryAST = parseConstantExpr();
        VERIFY_NEXT_TOKEN(rparen);
    }
    return primaryAST;
}

/*
 * moduleBody ::= statement moduleBody | epsilon
 * */
void Parser::parseModuleBody() {
    auto [lookAheadReady, lookAheadTokenData] = lookAhead();
    if (!lookAheadReady) {
        std::get<2>(lookAheadTokenData) = -1;
        std::get<3>(lookAheadTokenData) = -1;
        errorParsing(lookAheadTokenData, "Unexpected EOF");
    }
    auto tkEnum = std::get<0>(lookAheadTokenData);
    if (tkEnum == TOKEN_input || tkEnum == TOKEN_output) {
        parseInputOutputStatement();
    } else if (tkEnum == TOKEN_assign) {
        parseCombAssignStatement();
    } else if (tkEnum == TOKEN_wire || tkEnum == TOKEN_reg) {
        parseRegWireStatement();
    } else if (tkEnum == TOKEN_always) {
        hardwareModule.addAlwaysBlock(parseAlwaysBlock());
    } else {
        if (tkEnum == TOKEN_endmodule) {
            return;
        }
        errorParsing(lookAheadTokenData, "Unexpected token");
    }
    parseModuleBody();
}

/*
 * output_stmt ::= "input"|"output" var1 "," var2 ";"
 * */
void Parser::parseInputOutputStatement() {
    auto [_, inOutToken] = nextToken();
    PortDirection direction{};
    if (std::get<0>(inOutToken) == TOKEN_input) {
        direction = PortDirection::Input;
    } else if (std::get<0>(inOutToken) == TOKEN_output) {
        direction = PortDirection::Output;
    }
    while (true) {
        auto [_1, identifierToken] = VERIFY_NEXT_TOKEN(identifier);
        auto port = hardwareModule.getModuleIOPortByName(std::get<1>(identifierToken));
        port->setPortDirection(direction);
        auto [_2, commaOrSemicolonToken] = nextToken();
        if (std::get<0>(commaOrSemicolonToken) == TOKEN_semicolon) {
            return;
        } else if (std::get<0>(commaOrSemicolonToken) != TOKEN_comma) {
            errorParsing(commaOrSemicolonToken, "Unexpected token", "comma");
        }
    }
}

/*
 * assign_stmt ::= "assign" id slicing? "=" HDLExpression ";"
 *             ||= "assign" "{" id slicing? "," + "}" "=" HDLExpression ";"
 * */
void Parser::parseCombAssignStatement() {
    VERIFY_NEXT_TOKEN(assign);
    auto [_, identifierOrLbraceToken] = nextToken();
    if (std::get<0>(identifierOrLbraceToken) == TOKEN_identifier) {
        auto identifierToken = identifierOrLbraceToken;
        auto [_1, slicingOrEqualToken] = lookAhead();
        PortSlicingAST slicingAst{-1, -1};
        if (std::get<0>(slicingOrEqualToken) == TOKEN_lbracket) {
            errorParsing(slicingOrEqualToken,
                         "The parser supports sub-word assignment but the backend does not. "
                         "The verilog subset supported by this tool has deprecated that feature, "
                         "which is not a good style. Please try using concat.");
        }
        VERIFY_NEXT_TOKEN(single_eq);
        auto hdlExpr = parseHDLExpression();
        VERIFY_NEXT_TOKEN(semicolon);

        if (slicingAst.isTrivial()) {
            hardwareModule.addCircuitConnection(CircuitConnection{std::get<1>(identifierToken), std::move(hdlExpr)});
        } else {
            hardwareModule.addCircuitConnection(
                    CircuitConnection{std::get<1>(identifierToken), slicingAst, std::move(hdlExpr)});
        }
    } else if (std::get<0>(identifierOrLbraceToken) == TOKEN_lbrace) {
        std::vector<std::pair<std::string, PortSlicingAST>> lhsIdentifiers;
        while (true) {
            auto [_1, identifierToken] = VERIFY_NEXT_TOKEN(identifier);
            auto [_2, slicingOrCommaOrRbraceToken] = lookAhead();
            PortSlicingAST slicingAst{-1, -1};
            if (std::get<0>(slicingOrCommaOrRbraceToken) == TOKEN_lbracket) {
                errorParsing(slicingOrCommaOrRbraceToken,
                             "The parser supports sub-word assignment but the backend does not. "
                             "The verilog subset supported by this tool has deprecated that feature, "
                             "which is not a good style. Please try using concat.");
            } else if (std::get<0>(slicingOrCommaOrRbraceToken) == TOKEN_comma) {
                nextToken();
            } else if (std::get<0>(slicingOrCommaOrRbraceToken) == TOKEN_rbrace) {
                nextToken();
                lhsIdentifiers.emplace_back(std::get<1>(identifierToken), slicingAst);
                break;
            } else {
                errorParsing(slicingOrCommaOrRbraceToken, "Unexpected token");
            }
            lhsIdentifiers.emplace_back(std::get<1>(identifierToken), slicingAst);
        }
        VERIFY_NEXT_TOKEN(single_eq);
        auto hdlExpr = parseHDLExpression();
        VERIFY_NEXT_TOKEN(semicolon);

        std::reverse(lhsIdentifiers.begin(), lhsIdentifiers.end());

        hardwareModule.addCircuitConnection(
                CircuitConnection{std::move(lhsIdentifiers),
                                  std::move(hdlExpr)}
        );
    }
}

/*
 * reg_wire_stmt ::= "wire" slicing? identifier "=" HDLExpression ";"
 *               ||= "reg" slicing? identifier ";"
 * */
void Parser::parseRegWireStatement() {
    auto [_, wireOrRegToken] = nextToken();
    auto [_1, identifierOrSlicingToken] = lookAhead();
    decltype(identifierOrSlicingToken) identifierToken;
    PortSlicingAST slicing{0, 0};
    if (std::get<0>(identifierOrSlicingToken) == TOKEN_lbracket) {
        slicing = parsePortSlicing();
        identifierToken = std::get<1>(nextToken());
    } else {
        identifierToken = identifierOrSlicingToken;
        nextToken();
    }
    if (std::get<0>(wireOrRegToken) == TOKEN_wire) {
        auto wire = std::make_shared<CircuitSymbolWire>(std::get<1>(identifierToken), slicing);
        wire->setIsWire();
        hardwareModule.circuitSymbols.push_back(wire);
        auto [_2, equalOrSemicolonToken] = nextToken();
        if (std::get<0>(equalOrSemicolonToken) == TOKEN_single_eq) {
            auto hdlExpr = parseHDLExpression();
            hardwareModule.addCircuitConnection(CircuitConnection{std::get<1>(identifierToken), std::move(hdlExpr)});
            VERIFY_NEXT_TOKEN(semicolon);
        }
    } else if (std::get<0>(wireOrRegToken) == TOKEN_reg) {
        auto reg = std::make_shared<CircuitSymbolReg>(std::get<1>(identifierToken), slicing);
        hardwareModule.circuitSymbols.push_back(reg);
        hardwareModule.registers.push_back(reg);
        VERIFY_NEXT_TOKEN(semicolon);
    } else {
        errorParsing(wireOrRegToken, "Expecting wire or reg");
    }
}

std::unique_ptr<HDLExpressionAST> Parser::parseHDLExpression() {
    /* (a ^ b) | (c | d) & (~c)[2] && (a[1] || a[2]) */
    std::vector<std::unique_ptr<HDLExpressionAST>> astStack;
    std::vector<std::pair<VeriPythonTokens, int>> operatorStack;
    LexTokenType outToken;
    while (true) {
        int currentPrecedence = 0;
        if (!operatorStack.empty()) {
            currentPrecedence = operatorStack[operatorStack.size() - 1].second;
        }
        auto [tokenReady, lookAheadToken] = lookAhead();
        if (!tokenReady) {
            outToken = lookAheadToken;
            std::get<2>(outToken) = -1;
            std::get<3>(outToken) = -1;
            break;
        }
        auto tkEnum = std::get<0>(lookAheadToken);
        if (tkEnum == TOKEN_const_number || tkEnum == TOKEN_sized_number ||
            tkEnum == TOKEN_identifier || tkEnum == TOKEN_lparen ||
            tkEnum == TOKEN_lbrace) {
            auto ast = parseHDLPrimary();
            astStack.push_back(std::move(ast));

            /* 继续前瞻，根据后一个 operator 决定是否要对当前栈上元素进行合并 */
            auto [nextOpReady, nextOperator] = lookAhead();
            int nextPrecedence = getHDLOperatorPrecedence(nextOperator);
            /* 如果下一个token不是operator，说明已经到达了结尾，直接当作 -1 处理就行 */
            /* 合并栈上元素，保证左结合性 */
            if (currentPrecedence >= nextPrecedence) {
                while (!operatorStack.empty()) {
                    VeriPythonTokens currentOperator = operatorStack[operatorStack.size() - 1].first;
                    auto merge_ast = std::make_unique<HDLExpressionAST>(currentOperator);
                    if (currentOperator == TOKEN_logical_not || currentOperator == TOKEN_bitwise_not) {
                        merge_ast->children.push_back(std::move(astStack[astStack.size() - 1]));
                        astStack.pop_back();
                    } else if (currentOperator == TOKEN_colon) {
                        merge_ast = std::make_unique<HDLMuxAST>();
                        merge_ast->children.push_back(std::move(astStack[astStack.size() - 3]));
                        merge_ast->children.push_back(std::move(astStack[astStack.size() - 2]));
                        merge_ast->children.push_back(std::move(astStack[astStack.size() - 1]));
                        astStack.pop_back();
                        astStack.pop_back();
                        astStack.pop_back();
                        operatorStack.pop_back();
                    } else if (currentOperator != TOKEN_question) {
                        merge_ast->children.push_back(std::move(astStack[astStack.size() - 2]));
                        merge_ast->children.push_back(std::move(astStack[astStack.size() - 1]));
                        astStack.pop_back();
                        astStack.pop_back();
                    }
                    if (currentOperator != TOKEN_question) {
                        operatorStack.pop_back();
                        astStack.push_back(std::move(merge_ast));
                    }
                    if (currentPrecedence == nextPrecedence) {
                        outToken = nextOperator;
                        break;
                    }
                }
            }
        } else {
            int precedence = getHDLOperatorPrecedence(lookAheadToken);
            if (precedence == -1) {
                outToken = lookAheadToken;
                break;
            }
            nextToken();
            operatorStack.emplace_back(tkEnum, precedence);
        }
    }

    if (!operatorStack.empty() || astStack.size() != 1) {
        errorParsing(outToken, "Failed to parse HDL expression");
    }
    return std::move(astStack[0]);
}

/*
 * hdlPrimary ::= (const_number | sized_number | identifier | "(" hdlExpression ")" | "{" hdlExpression "," + "}") portSlicing?
 * */
std::unique_ptr<HDLExpressionAST> Parser::parseHDLPrimary() {
    auto [lookAheadReady, lookAheadTokenData] = lookAhead();
    if (!lookAheadReady) {
        std::get<2>(lookAheadTokenData) = -1;
        std::get<3>(lookAheadTokenData) = -1;
        errorParsing(lookAheadTokenData, "Unexpected EOF");
    }
    decltype(parseHDLPrimary()) primaryAST;
    auto tkEnum = std::get<0>(lookAheadTokenData);
    auto tkData = std::get<1>(lookAheadTokenData);
    if (tkEnum == TOKEN_const_number) {
        nextToken();
        auto ast = static_cast<HDLExpressionAST *>(new HDLPrimaryAST{std::stoi(tkData)});
        primaryAST = std::unique_ptr<HDLExpressionAST>(ast);
    } else if (tkEnum == TOKEN_sized_number) {
        nextToken();
        /* 4'd8000_0000 */
        const std::string &sizedNumberString = tkData;
        char *sizedNumberCstr = strdup(sizedNumberString.c_str());

        char *widthStr = strtok(sizedNumberCstr, "'");
        int width = std::atoi(widthStr);

        char *dataStr = strtok(nullptr, "'");
        int base = dataStr[0] == 'd' ? 10 : (dataStr[0] == 'h' ? 16 : 2);
        std::string realData;
        for (std::size_t i = 1; i < strlen(dataStr); i++) {
            if (dataStr[i] != '_') {
                realData += dataStr[i];
            }
        }
        int data = std::stoi(realData, nullptr, base);

        auto ast = static_cast<HDLExpressionAST *>(new HDLPrimaryAST{data, width, base});
        primaryAST = std::unique_ptr<HDLExpressionAST>(ast);
        free(sizedNumberCstr);
    } else if (tkEnum == TOKEN_identifier) {
        nextToken();
        auto ast = static_cast<HDLExpressionAST *>(new HDLPrimaryAST{tkData});
        primaryAST = std::unique_ptr<HDLExpressionAST>(ast);
    } else if (tkEnum == TOKEN_lparen) {
        nextToken();
        primaryAST = parseHDLExpression();
        VERIFY_NEXT_TOKEN(rparen);
    } else if (tkEnum == TOKEN_lbrace) {
        nextToken();
        auto ast = std::make_unique<HDLConcatAST>();
        while (true) {
            auto hdlExpr = parseHDLExpression();
            ast->children.push_back(std::move(hdlExpr));
            auto [_, commaOrRbraceToken] = nextToken();
            if (std::get<0>(commaOrRbraceToken) == TOKEN_rbrace) {
                break;
            } else if (std::get<0>(commaOrRbraceToken) != TOKEN_comma) {
                errorParsing(commaOrRbraceToken, "Unexpected token", "comma");
            }
        }
        std::reverse(ast->children.begin(), ast->children.end());
        primaryAST = std::move(ast);
    }

    auto [_, isLBracket] = lookAhead();
    if (std::get<0>(isLBracket) == TOKEN_lbracket) {
        primaryAST->setExprSlicing(parsePortSlicing());
    }

    return primaryAST;
}

/*
 * always_block ::= "always" "@" "(" sensitive_list ")" "begin" always_body "end"
 * */
std::unique_ptr<AlwaysBlockAST> Parser::parseAlwaysBlock() {
    VERIFY_NEXT_TOKEN(always);
    VERIFY_NEXT_TOKEN(at);
    VERIFY_NEXT_TOKEN(lparen);
    auto sensList = parseSensitiveList();
    VERIFY_NEXT_TOKEN(rparen);
    VERIFY_NEXT_TOKEN(begin);
    auto block = parseAlwaysBlockBody();
    VERIFY_NEXT_TOKEN(end);
    return std::make_unique<AlwaysBlockAST>(sensList, std::move(block));
}

std::vector<std::pair<TriggerEdgeType, std::string>> Parser::parseSensitiveList() {
    std::vector<std::pair<TriggerEdgeType, std::string>> lists;
    while (true) {
        auto [_, posedgeOrNegedgeOrStarToken] = nextToken();
        auto triggerType = TriggerEdgeType::NOT_SPECIFIED;
        auto tkEnum = std::get<0>(posedgeOrNegedgeOrStarToken);
        if (tkEnum == TOKEN_posedge) {
            triggerType = TriggerEdgeType::POSITIVE_EDGE;
        } else if (tkEnum == TOKEN_negedge) {
            triggerType = TriggerEdgeType::NEGATIVE_EDGE;
        } else if (tkEnum == TOKEN_op_mul) {
            triggerType = TriggerEdgeType::NOT_SPECIFIED;
            lists.emplace_back(triggerType, "*");
            break;
        } else {
            errorParsing(posedgeOrNegedgeOrStarToken, "Expecting posedge or negedge or *");
        }
        auto [_1, identifierToken] = VERIFY_NEXT_TOKEN(identifier);
        lists.emplace_back(triggerType, std::get<1>(identifierToken));
        auto [_2, lookAheadToken] = lookAhead();
        if (std::get<0>(lookAheadToken) == TOKEN_senslist_or) {
            nextToken();
            continue;
        } else if (std::get<0>(lookAheadToken) == TOKEN_rparen) {
            break;
        } else {
            errorParsing(lookAheadToken, "Expecting 'or' or ')'");
        }
    }

    return lists;
}

/*
 * alwaysBlockBody ::= always_stmt*
 * */
std::unique_ptr<AlwaysBlockBodyAST> Parser::parseAlwaysBlockBody() {
    auto ast = std::make_unique<AlwaysBlockBodyAST>(nullptr);
    while (true) {
        auto [_, lookAheadToken] = lookAhead();
        if (std::get<0>(lookAheadToken) == TOKEN_end) {
            break;
        } else {
            ast->children.push_back(parseAlwaysBlockBodyStatement());
        }
    }

    return ast;
}

/*
 * always_stmt ::= ifBlock | caseBlock | nonBlockingAssign
 * */
std::unique_ptr<AlwaysBlockBodyAST> Parser::parseAlwaysBlockBodyStatement() {
    auto [_, lookAheadToken] = lookAhead();
    auto tkEnum = std::get<0>(lookAheadToken);
    if (tkEnum == TOKEN_if) {
        return parseIfBlock();
    } else if (tkEnum == TOKEN_case || tkEnum == TOKEN_casez ||
               tkEnum == TOKEN_casex) {
        errorParsing(lookAheadToken,
                     "The verilog subset supported by this tool does not support case. Try using if instead.");
    } else {
        auto ptr = static_cast<AlwaysBlockBodyAST *>(new NonBlockingAssignAST{parseNonBlockingAssignment()});
        return std::unique_ptr<AlwaysBlockBodyAST>(ptr);
    }
    errorParsing(lookAheadToken, "The HDL block cannot be recognized by this tool");
    return nullptr;
}

std::unique_ptr<AlwaysBlockBodyAST> Parser::parseIfBlock() {
    auto [_, ifToken] = VERIFY_NEXT_TOKEN(if);
    VERIFY_NEXT_TOKEN(lparen);
    auto condition = parseHDLExpression();
    VERIFY_NEXT_TOKEN(rparen);

    auto ast = std::make_unique<AlwaysBlockBodyAST>(std::move(condition));
    ast->nodeType = "__hw_always_block_body_if_block__";

    auto [_1, beginOrOtherToken] = lookAhead();
    if (std::get<0>(beginOrOtherToken) != TOKEN_begin) {
        /* 只能有一个表达式 */
        if (std::get<0>(beginOrOtherToken) == TOKEN_if) {
            ast->children.push_back(parseIfBlock());
        } else {
            ast->children.push_back(parseAlwaysBlockBodyStatement());
        }
    } else {
        nextToken();
        ast->children.push_back(parseAlwaysBlockBody());
        VERIFY_NEXT_TOKEN(end);
    }
    auto [_2, elseOrOtherToken] = lookAhead();
    if (std::get<0>(elseOrOtherToken) == TOKEN_else) {
        nextToken();
        auto [_3, beginOrOtherTokenElse] = lookAhead();
        if (std::get<0>(beginOrOtherTokenElse) == TOKEN_begin) {
            nextToken();
            ast->children.push_back(parseAlwaysBlockBody());
            VERIFY_NEXT_TOKEN(end);
        } else {
            ast->children.push_back(parseAlwaysBlockBodyStatement());
        }
    }

    return ast;
}

/*
 * non_blk_stmt ::= id slicing? "<=" HDLExpression ";"
 *             ||= "{" id slicing? "," + "}" "<=" HDLExpression ";"
 * */
CircuitConnection Parser::parseNonBlockingAssignment() {
    auto [_, identifierOrLbraceToken] = nextToken();
    if (std::get<0>(identifierOrLbraceToken) == TOKEN_identifier) {
        auto identifierToken = identifierOrLbraceToken;
        auto [_1, slicingOrEqualToken] = lookAhead();
        PortSlicingAST slicingAst{-1, -1};
        if (std::get<0>(slicingOrEqualToken) == TOKEN_lbracket) {
            slicingAst = parsePortSlicing();
        }
        VERIFY_NEXT_TOKEN(cond_le);
        auto hdlExpr = parseHDLExpression();
        VERIFY_NEXT_TOKEN(semicolon);

        if (slicingAst.isTrivial()) {
            return {std::get<1>(identifierToken), std::move(hdlExpr)};
        } else {
            return {std::get<1>(identifierToken), slicingAst, std::move(hdlExpr)};
        }
    } else if (std::get<0>(identifierOrLbraceToken) == TOKEN_lbrace) {
        std::vector<std::pair<std::string, PortSlicingAST>> lhsIdentifiers;
        while (true) {
            auto [_1, identifierToken] = VERIFY_NEXT_TOKEN(identifier);
            auto [_2, slicingOrCommaOrRbraceToken] = lookAhead();
            PortSlicingAST slicingAst{-1, -1};
            if (std::get<0>(slicingOrCommaOrRbraceToken) == TOKEN_lbracket) {
                slicingAst = parsePortSlicing();
                auto [_3, commaOrRbraceToken] = nextToken();
                if (std::get<0>(commaOrRbraceToken) == TOKEN_rbrace) {
                    break;
                } else if (std::get<0>(commaOrRbraceToken) != TOKEN_comma) {
                    errorParsing(commaOrRbraceToken, "Unexpected token", "comma");
                }
            } else if (std::get<0>(slicingOrCommaOrRbraceToken) == TOKEN_comma) {
                nextToken();
            } else if (std::get<0>(slicingOrCommaOrRbraceToken) == TOKEN_rbrace) {
                nextToken();
                lhsIdentifiers.emplace_back(std::get<1>(identifierToken), slicingAst);
                break;
            } else {
                errorParsing(slicingOrCommaOrRbraceToken, "Unexpected token");
            }
            lhsIdentifiers.emplace_back(std::get<1>(identifierToken), slicingAst);
        }
        VERIFY_NEXT_TOKEN(cond_le);
        auto hdlExpr = parseHDLExpression();
        VERIFY_NEXT_TOKEN(semicolon);

        std::reverse(lhsIdentifiers.begin(), lhsIdentifiers.end());

        return {std::move(lhsIdentifiers), std::move(hdlExpr)};
    }
    errorParsing(identifierOrLbraceToken, "Unexpected token");
    return {"error", std::make_unique<HDLExpressionAST>(TOKEN_lbrace)};
}

Parser::~Parser() {
    fclose(yyin);
}

std::tuple<bool, Parser::LexTokenType> Parser::lookAhead() {
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

std::tuple<bool, Parser::LexTokenType> Parser::nextToken() {
    if (tokenBuffer.empty()) {
        int ret = yylex();
        while (ret == TOKEN_single_line_comment_end ||
               ret == TOKEN_single_line_comment ||
               ret == TOKEN_multi_line_comment_start) {
            if (ret == TOKEN_single_line_comment) {
                while (ret != TOKEN_single_line_comment_end) {
                    ret = yylex();
                }
            } else if (ret == TOKEN_multi_line_comment_start) {
                while (ret != TOKEN_multi_line_comment_end) {
                    ret = yylex();
                }
            }
            ret = yylex();
        }
        if (ret == 0) {
            return {false, {TOKEN_assign, "", -1, -1}};
        }
        std::string data{yylval.str};
        free(yylval.str);
        LexTokenType tokenData = {VeriPythonTokens(ret), data, yy_lineno, yycolumn};
        return {true, tokenData};
    } else {
        auto tokenData = tokenBuffer.front();
        tokenBuffer.pop();
        return {true, tokenData};
    }
}

std::tuple<bool, Parser::LexTokenType>
Parser::nextToken(enum VeriPythonTokens expectedTokenEnum, const std::string &expectTokenName) {
    auto tokenTuple = nextToken();
    if (std::get<0>(tokenTuple) && std::get<0>(std::get<1>(tokenTuple)) != expectedTokenEnum) {
        errorParsing(std::get<1>(tokenTuple), "Unexpected token", expectTokenName);
    }
    return tokenTuple;
}

void Parser::printErrorToken(std::ostream &out, const LexTokenType &errorToken, const std::string &msg) {
    int lineNumber = std::get<2>(errorToken);
    int columnNumber = std::get<3>(errorToken);
    if (lineNumber == -1 || columnNumber == -1) {
        return;
    }
    out << sourceFileName << ":" << lineNumber << ":"
        << columnNumber << ": " << msg << "\n";
    if (lineNumber && lineNumber > (int) sourceFileLines.size()) {
        throw std::range_error("Line number invalid");
    }
    out << sourceFileLines[lineNumber - 1] << "\n";
    for (int i = 1; i < columnNumber; i++) {
        out << " ";
    }
    out << "^" << std::endl;
}

void Parser::warningParsing(const LexTokenType &errorToken, const std::string &message) {
    printErrorToken(std::cout, errorToken, "warning: " + message);
}

void Parser::errorParsing(const LexTokenType &errorToken,
                          const std::string &message,
                          const std::string &expectTokenName) {
    printErrorToken(std::cerr, errorToken, "error: " + message + ". Expecting: " + expectTokenName);
    throw std::runtime_error("Code syntax error");
}

int Parser::getConstantOperatorPrecedence(LexTokenType &token) {
    if (operatorPrecedence.count(std::get<0>(token)) == 0) {
        return -1;
    }
    auto pred = operatorPrecedence[std::get<0>(token)];
    return pred >= 10 ? pred : -1;
}

int Parser::getHDLOperatorPrecedence(LexTokenType &token) {
    if (operatorPrecedence.count(std::get<0>(token)) == 0) {
        return -1;
    }
    return operatorPrecedence[std::get<0>(token)];
}
