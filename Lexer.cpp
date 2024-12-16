//
// Created by hanyuan on 2024/11/16.
//
#include "Lexer.h"
#include <string>
#include <sstream>

extern "C" {
int yylex();
extern char *yytext;
extern FILE *yyin;
}

const char *tokenToNameArray[] = {
        "module",
        "endmodule",
        "output",
        "input",
        "assign",
        "wire",
        "always",
        "posedge",
        "negedge",
        "senslist_or",
        "begin",
        "end",
        "case",
        "casez",
        "casex",
        "endcase",
        "if",
        "else",
        "reg",
        "comma",
        "semicolon",
        "colon",
        "question",
        "identifier",
        "const_number",
        "sized_number",
        "lparen",
        "rparen",
        "lbrace",
        "rbrace",
        "lbracket",
        "rbracket",
        "op_add",
        "op_sub",
        "op_mul",
        "op_div",
        "op_mod",
        "arith_lshift",
        "logical_lshift",
        "arith_rshift",
        "logical_rshift",
        "cond_eq",
        "cond_ne",
        "cond_ge",
        "cond_gt",
        "cond_le",
        "cond_lt",
        "single_eq",
        "bitwise_and",
        "bitwise_or",
        "bitwise_not",
        "bitwise_xor",
        "logical_and",
        "logical_or",
        "logical_not",
        "at",
        "single_line_comment",
        "multi_line_comment_start",
        "multi_line_comment_end",
};

std::string getAllTokens(const std::string &filename) {
    std::stringstream out;
    yyin = fopen(filename.c_str(), "r");
    out << "[";
    while (true) {
        int ret = yylex();
        if (ret == 0) {
            break;
        }
        std::string data{yylval.str};
        if (ret != TOKEN_single_line_comment_end) {
            free(yylval.str);
        }
        out << "{\n"
            << R"(  "token": ")" << tokenToNameArray[ret + 1024] << "\",\n"
            << R"(  "data": ")" << data << "\"\n"
            << "},\n";
    }
    auto ret = out.str();
    ret.pop_back();
    ret.pop_back();
    ret += "\n]";
    return ret;
}