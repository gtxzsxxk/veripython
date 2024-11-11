//
// Created by hanyuan on 2024/11/1.
//

#include "Parser.h"
#include <iostream>
#include <cstdio>
#include <exception>
#include <vector>
#include <algorithm>

#define VERIFY_NEXT_TOKEN(name)         nextToken(TOKEN_##name, #name)

extern "C" {
int yylex();
extern char *yytext;
extern FILE *yyin;
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
 *            ||= "input"|"output" ("wire"|"reg")? portSlicing id
 *            ||= "input"|"output" ("wire"|"reg") id
 * */
void Parser::parseModulePort() {
    auto [tokenReady, idOrInOut] = lookAhead();
    if (!tokenReady) {
        errorParsing("Unexpected EOF");
    }
    if (idOrInOut.first == TOKEN_identifier) {
        auto [_, identifierToken] = nextToken();
        hardwareModule.ioPorts.emplace_back(new ModuleIOPort{identifierToken.second});
    } else if (idOrInOut.first == TOKEN_input || idOrInOut.first == TOKEN_output) {
        nextToken();
        auto [_, wireOrRegOrOther] = lookAhead();
        decltype(wireOrRegOrOther) idOrPortSlicing;
        if(wireOrRegOrOther.first == TOKEN_wire) {
            nextToken();
            idOrPortSlicing = std::get<1>(lookAhead());
        } else if (wireOrRegOrOther.first == TOKEN_reg) {
            nextToken();
            idOrPortSlicing = std::get<1>(lookAhead());
            errorParsing("Not supported");
        } else {
            idOrPortSlicing = wireOrRegOrOther;
        }
        auto direction = idOrInOut.first == TOKEN_input ? PortDirection::Input : PortDirection::Output;
        if (idOrPortSlicing.first == TOKEN_identifier) {
            auto [_, identifierToken] = nextToken();
            hardwareModule.ioPorts.emplace_back(new ModuleIOPort{direction, identifierToken.second});
        } else if (idOrPortSlicing.first == TOKEN_lbracket) {
            auto slicing = parsePortSlicing();
            auto [_, identifierToken] = nextToken();
            hardwareModule.ioPorts.emplace_back(new ModuleIOPort{direction, slicing, identifierToken.second});
        } else {
            errorParsing("Unexpected Token after input/output");
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
        errorParsing("Unexpected EOF");
    }

    PortSlicingAST portSlicing{0};
    if (lookAheadToken.first == TOKEN_rbracket) {
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
                        break;
                    }
                }
            }
        } else {
            int precedence = getConstantOperatorPrecedence(lookAheadToken);
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
    return std::move(astStack[0]);
}

/*
 * constantPrimary ::= const_number | "(" constantExpression ")"
 * */
std::unique_ptr<ConstantExpressionAST> Parser::parseConstantPrimary() {
    auto [lookAheadReady, lookAheadTokenData] = lookAhead();
    if (!lookAheadReady) {
        errorParsing("Unexpected EOF");
    }
    decltype(parseConstantPrimary()) primaryAST;
    if (lookAheadTokenData.first == TOKEN_const_number) {
        nextToken();
        auto ast = static_cast<ConstantExpressionAST *>(new ConstantNumberAST{std::stoi(lookAheadTokenData.second)});
        primaryAST = std::unique_ptr<ConstantExpressionAST>{ast};
    } else if (lookAheadTokenData.first == TOKEN_lparen) {
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
        errorParsing("Unexpected EOF");
    }
    if (lookAheadTokenData.first == TOKEN_input || lookAheadTokenData.first == TOKEN_output) {
        parseInputOutputStatement();
    } else if (lookAheadTokenData.first == TOKEN_assign) {
        parseCombAssignStatement();
    } else if (lookAheadTokenData.first == TOKEN_wire || lookAheadTokenData.first == TOKEN_reg) {
        parseRegWireStatement();
    } else if(lookAheadTokenData.first == TOKEN_always) {
        auto ast = parseAlwaysBlock();
        std::cout << ast->nodeType << std::endl;
    } else {
        if (lookAheadTokenData.first == TOKEN_endmodule) {
            return;
        }
        errorParsing("Unexpected token");
    }
    parseModuleBody();
}

/*
 * output_stmt ::= "input"|"output" var1 "," var2 ";"
 * */
void Parser::parseInputOutputStatement() {
    auto [_, inOutToken] = nextToken();
    PortDirection direction{};
    if (inOutToken.first == TOKEN_input) {
        direction = PortDirection::Input;
    } else if (inOutToken.first == TOKEN_output) {
        direction = PortDirection::Output;
    }
    while (true) {
        auto [_1, identifierToken] = VERIFY_NEXT_TOKEN(identifier);
        auto port = hardwareModule.getModuleIOPortByName(identifierToken.second);
        port->setPortDirection(direction);
        auto [_2, commaOrSemicolonToken] = nextToken();
        if (commaOrSemicolonToken.first == TOKEN_semicolon) {
            return;
        } else if (commaOrSemicolonToken.first != TOKEN_comma) {
            errorParsing("Expecting comma");
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
    if (identifierOrLbraceToken.first == TOKEN_identifier) {
        auto identifierToken = identifierOrLbraceToken;
        auto [_1, slicingOrEqualToken] = lookAhead();
        PortSlicingAST slicingAst{-1, -1};
        if (slicingOrEqualToken.first == TOKEN_lbracket) {
            slicingAst = parsePortSlicing();
        }
        VERIFY_NEXT_TOKEN(single_eq);
        auto hdlExpr = parseHDLExpression();
        VERIFY_NEXT_TOKEN(semicolon);

        if (slicingAst.isTrivial()) {
            hardwareModule.addCircuitConnection(CircuitConnection{identifierToken.second, std::move(hdlExpr)});
        } else {
            hardwareModule.addCircuitConnection(
                    CircuitConnection{identifierToken.second, slicingAst, std::move(hdlExpr)});
        }
    } else if (identifierOrLbraceToken.first == TOKEN_lbrace) {
        std::vector<std::pair<std::string, PortSlicingAST>> lhsIdentifiers;
        while (true) {
            auto [_1, identifierToken] = VERIFY_NEXT_TOKEN(identifier);
            auto [_2, slicingOrCommaOrRbraceToken] = lookAhead();
            PortSlicingAST slicingAst{-1, -1};
            if (slicingOrCommaOrRbraceToken.first == TOKEN_lbracket) {
                slicingAst = parsePortSlicing();
                auto [_3, commaOrRbraceToken] = nextToken();
                if (commaOrRbraceToken.first == TOKEN_rbrace) {
                    break;
                } else if (commaOrRbraceToken.first != TOKEN_comma) {
                    errorParsing("Unexpected token");
                }
            } else if (slicingOrCommaOrRbraceToken.first == TOKEN_comma) {
                nextToken();
            } else if (slicingOrCommaOrRbraceToken.first == TOKEN_rbrace) {
                nextToken();
                lhsIdentifiers.emplace_back(identifierToken.second, slicingAst);
                break;
            } else {
                errorParsing("Unexpected token");
            }
            lhsIdentifiers.emplace_back(identifierToken.second, slicingAst);
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
    if (identifierOrSlicingToken.first == TOKEN_lbracket) {
        slicing = parsePortSlicing();
        identifierToken = std::get<1>(nextToken());
    } else {
        identifierToken = identifierOrSlicingToken;
        nextToken();
    }
    if (wireOrRegToken.first == TOKEN_wire) {
        auto wire = std::make_shared<CircuitSymbolWire>(identifierToken.second, slicing);
        hardwareModule.circuitSymbols.push_back(wire);
        auto [_2, equalOrSemicolonToken] = nextToken();
        if (equalOrSemicolonToken.first == TOKEN_single_eq) {
            auto hdlExpr = parseHDLExpression();
            hardwareModule.addCircuitConnection(CircuitConnection{identifierToken.second, std::move(hdlExpr)});
            VERIFY_NEXT_TOKEN(semicolon);
        }
    } else if (wireOrRegToken.first == TOKEN_reg) {
        auto reg = std::make_shared<CircuitSymbolReg>(identifierToken.second, slicing);
        hardwareModule.circuitSymbols.push_back(reg);
        VERIFY_NEXT_TOKEN(semicolon);
    } else {
        errorParsing("Expecting wire or reg");
    }
}

std::unique_ptr<HDLExpressionAST> Parser::parseHDLExpression() {
    /* (a ^ b) | (c | d) & (~c)[2] && (a[1] || a[2]) */
    std::vector<std::unique_ptr<HDLExpressionAST>> astStack;
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
        if (lookAheadToken.first == TOKEN_const_number || lookAheadToken.first == TOKEN_sized_number ||
            lookAheadToken.first == TOKEN_identifier || lookAheadToken.first == TOKEN_lparen ||
            lookAheadToken.first == TOKEN_lbrace) {
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
                        merge_ast = std::make_unique<HDLMuxAST>(std::move(astStack[astStack.size() - 3]));
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
                        break;
                    }
                }
            }
        } else {
            int precedence = getHDLOperatorPrecedence(lookAheadToken);
            if (precedence == -1) {
                goto out;
            }
            nextToken();
            operatorStack.emplace_back(lookAheadToken.first, precedence);
        }
    }

    out:
    if (!operatorStack.empty() || astStack.size() != 1) {
        errorParsing("Failed to parse HDL expression");
    }
    return std::move(astStack[0]);
}

/*
 * hdlPrimary ::= (const_number | sized_number | identifier | "(" hdlExpression ")" | "{" hdlExpression "," + "}") portSlicing?
 * */
std::unique_ptr<HDLExpressionAST> Parser::parseHDLPrimary() {
    auto [lookAheadReady, lookAheadTokenData] = lookAhead();
    if (!lookAheadReady) {
        errorParsing("Unexpected EOF");
    }
    decltype(parseHDLPrimary()) primaryAST;
    if (lookAheadTokenData.first == TOKEN_const_number) {
        nextToken();
        auto ast = static_cast<HDLExpressionAST *>(new HDLPrimaryAST{std::stoi(lookAheadTokenData.second)});
        primaryAST = std::unique_ptr<HDLExpressionAST>(ast);
    } else if (lookAheadTokenData.first == TOKEN_sized_number) {
        nextToken();
        /* 4'd8000_0000 */
        std::string sizedNumberString = lookAheadTokenData.second;
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
    } else if (lookAheadTokenData.first == TOKEN_identifier) {
        nextToken();
        auto ast = static_cast<HDLExpressionAST *>(new HDLPrimaryAST{lookAheadTokenData.second});
        primaryAST = std::unique_ptr<HDLExpressionAST>(ast);
    } else if (lookAheadTokenData.first == TOKEN_lparen) {
        nextToken();
        primaryAST = parseHDLExpression();
        VERIFY_NEXT_TOKEN(rparen);
    } else if (lookAheadTokenData.first == TOKEN_lbrace) {
        nextToken();
        auto ast = std::make_unique<HDLConcatAST>();
        while (true) {
            auto hdlExpr = parseHDLExpression();
            ast->children.push_back(std::move(hdlExpr));
            auto [_, commaOrRbraceToken] = nextToken();
            if (commaOrRbraceToken.first == TOKEN_rbrace) {
                break;
            } else if (commaOrRbraceToken.first != TOKEN_comma) {
                errorParsing("Expecting comma");
            }
        }
        std::reverse(ast->children.begin(), ast->children.end());
        primaryAST = std::move(ast);
    }

    auto [_, isLBracket] = lookAhead();
    if (isLBracket.first == TOKEN_lbracket) {
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
        if (posedgeOrNegedgeOrStarToken.first == TOKEN_posedge) {
            triggerType = TriggerEdgeType::POSITIVE_EDGE;
        } else if (posedgeOrNegedgeOrStarToken.first == TOKEN_negedge) {
            triggerType = TriggerEdgeType::NEGATIVE_EDGE;
        } else if (posedgeOrNegedgeOrStarToken.first != TOKEN_op_mul) {
            errorParsing("Expecting posedge or negedge or *");
        }
        auto [_1, identifierToken] = VERIFY_NEXT_TOKEN(identifier);
        lists.emplace_back(triggerType, identifierToken.second);
        auto [_2, lookAheadToken] = lookAhead();
        if (lookAheadToken.first == TOKEN_senslist_or) {
            nextToken();
            continue;
        } else if (lookAheadToken.first == TOKEN_rparen) {
            break;
        } else {
            errorParsing("Expecting 'or' or ')'");
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
        if (lookAheadToken.first == TOKEN_end) {
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
    if (lookAheadToken.first == TOKEN_if) {
        return parseIfBlock();
    }
//    else if (lookAheadToken.first == TOKEN_case) {
//        parseCaseBlock();
//    }
    else {
        auto ptr = static_cast<AlwaysBlockBodyAST*>(new NonBlockingAssignAST{parseNonBlockingAssignment()});
        return std::unique_ptr<AlwaysBlockBodyAST>(ptr);
    }
}

std::unique_ptr<AlwaysBlockBodyAST> Parser::parseIfBlock() {
    auto [_, ifToken] = VERIFY_NEXT_TOKEN(if);
    VERIFY_NEXT_TOKEN(lparen);
    auto condition = parseHDLExpression();
    VERIFY_NEXT_TOKEN(rparen);

    auto ast = std::make_unique<AlwaysBlockBodyAST>(std::move(condition));
    ast->nodeType = "__hw_always_block_body_if_block__";

    auto [_1, beginOrOtherToken] = lookAhead();
    if (beginOrOtherToken.first != TOKEN_begin) {
        /* 只能有一个表达式 */
        if (beginOrOtherToken.first == TOKEN_if) {
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
    if (elseOrOtherToken.first == TOKEN_else) {
        nextToken();
        auto [_3, beginOrOtherTokenElse] = lookAhead();
        if (beginOrOtherTokenElse.first == TOKEN_begin) {
            nextToken();
            ast->children.push_back(parseAlwaysBlockBody());
            VERIFY_NEXT_TOKEN(end);
        } else {
            ast->children.push_back(parseAlwaysBlockBodyStatement());
        }
    }

    return ast;
}

void Parser::parseCaseBlock() {

}

/*
 * non_blk_stmt ::= id slicing? "<=" HDLExpression ";"
 *             ||= "{" id slicing? "," + "}" "<=" HDLExpression ";"
 * */
CircuitConnection Parser::parseNonBlockingAssignment() {
    auto [_, identifierOrLbraceToken] = nextToken();
    if (identifierOrLbraceToken.first == TOKEN_identifier) {
        auto identifierToken = identifierOrLbraceToken;
        auto [_1, slicingOrEqualToken] = lookAhead();
        PortSlicingAST slicingAst{-1, -1};
        if (slicingOrEqualToken.first == TOKEN_lbracket) {
            slicingAst = parsePortSlicing();
        }
        VERIFY_NEXT_TOKEN(cond_le);
        auto hdlExpr = parseHDLExpression();
        VERIFY_NEXT_TOKEN(semicolon);

        if (slicingAst.isTrivial()) {
            return {identifierToken.second, std::move(hdlExpr)};
        } else {
            return {identifierToken.second, slicingAst, std::move(hdlExpr)};
        }
    } else if (identifierOrLbraceToken.first == TOKEN_lbrace) {
        std::vector<std::pair<std::string, PortSlicingAST>> lhsIdentifiers;
        while (true) {
            auto [_1, identifierToken] = VERIFY_NEXT_TOKEN(identifier);
            auto [_2, slicingOrCommaOrRbraceToken] = lookAhead();
            PortSlicingAST slicingAst{-1, -1};
            if (slicingOrCommaOrRbraceToken.first == TOKEN_lbracket) {
                slicingAst = parsePortSlicing();
                auto [_3, commaOrRbraceToken] = nextToken();
                if (commaOrRbraceToken.first == TOKEN_rbrace) {
                    break;
                } else if (commaOrRbraceToken.first != TOKEN_comma) {
                    errorParsing("Unexpected token");
                }
            } else if (slicingOrCommaOrRbraceToken.first == TOKEN_comma) {
                nextToken();
            } else if (slicingOrCommaOrRbraceToken.first == TOKEN_rbrace) {
                nextToken();
                lhsIdentifiers.emplace_back(identifierToken.second, slicingAst);
                break;
            } else {
                errorParsing("Unexpected token");
            }
            lhsIdentifiers.emplace_back(identifierToken.second, slicingAst);
        }
        VERIFY_NEXT_TOKEN(cond_le);
        auto hdlExpr = parseHDLExpression();
        VERIFY_NEXT_TOKEN(semicolon);

        std::reverse(lhsIdentifiers.begin(), lhsIdentifiers.end());

        return {std::move(lhsIdentifiers), std::move(hdlExpr)};
    }
    errorParsing("Unexpected token");
    return {"error", std::make_unique<HDLExpressionAST>(TOKEN_lbrace)};
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
    while (!tokenFetched.empty()) {
        auto tokenData = tokenFetched.front();
        tokenFetched.pop();
        std::cerr << tokenData.second << " ";
    }
    std::cerr << std::endl;
    throw std::runtime_error("Code syntax error");
}

int Parser::getConstantOperatorPrecedence(LexTokenType &token) {
    if (operatorPrecedence.count(token.first) == 0) {
        return -1;
    }
    auto pred = operatorPrecedence[token.first];
    return pred >= 10 ? pred : -1;
}

int Parser::getHDLOperatorPrecedence(LexTokenType &token) {
    if (operatorPrecedence.count(token.first) == 0) {
        return -1;
    }
    return operatorPrecedence[token.first];
}
