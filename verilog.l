%{
    #include <stdio.h>
    #include <string.h>
    #include "Lexer.h"
    #define RET_TOKEN(name)         { yylval.str = strdup(yytext); yycolumn = yycolumn_last; yycolumn_last += yyleng; return TOKEN_##name; }

    int yy_lineno = 1;
    int yycolumn = 1;
    int yycolumn_last = 1;
    void yyerror(const char *s);
%}

%%

"module"                                RET_TOKEN(module)
"endmodule"                             RET_TOKEN(endmodule)
"output"                                RET_TOKEN(output)
"input"                                 RET_TOKEN(input)
"assign"                                RET_TOKEN(assign)
"wire"                                  RET_TOKEN(wire)
"always"                                RET_TOKEN(always)
"begin"                                 RET_TOKEN(begin)
"end"                                   RET_TOKEN(end)
"casez"                                 RET_TOKEN(casez)
"casex"                                 RET_TOKEN(casex)
"case"                                  RET_TOKEN(case)
"endcase"                               RET_TOKEN(endcase)
"if"                                    RET_TOKEN(if)
"else"                                  RET_TOKEN(else)
"posedge"                               RET_TOKEN(posedge)
"negedge"                               RET_TOKEN(negedge)
"or"                                    RET_TOKEN(senslist_or)
"reg"                                   RET_TOKEN(reg)
","                                     RET_TOKEN(comma)
";"                                     RET_TOKEN(semicolon)
":"                                     RET_TOKEN(colon)
"?"                                     RET_TOKEN(question)
[A-Za-z\_][A-Za-z0-9\_]*                RET_TOKEN(identifier)
\-?[0-9]+'(d|h|b)[0-9a-f\_]+            RET_TOKEN(sized_number)
\-?[0-9]+                               RET_TOKEN(const_number)
"("                                     RET_TOKEN(lparen)
")"                                     RET_TOKEN(rparen)
"{"                                     RET_TOKEN(lbrace)
"}"                                     RET_TOKEN(rbrace)
"["                                     RET_TOKEN(lbracket)
"]"                                     RET_TOKEN(rbracket)
"+"                                     RET_TOKEN(op_add)
"-"                                     RET_TOKEN(op_sub)
"*"                                     RET_TOKEN(op_mul)
"/"                                     RET_TOKEN(op_div)
"%"                                     RET_TOKEN(op_mod)
"<<<"                                   RET_TOKEN(arith_lshift)
"<<"                                    RET_TOKEN(logical_lshift)
">>>"                                   RET_TOKEN(arith_rshift)
">>"                                    RET_TOKEN(logical_rshift)
"=="                                    RET_TOKEN(cond_eq)
"!="                                    RET_TOKEN(cond_ne)
">="                                    RET_TOKEN(cond_ge)
">"                                     RET_TOKEN(cond_gt)
"<="                                    RET_TOKEN(cond_le)
"<"                                     RET_TOKEN(cond_lt)
"="                                     RET_TOKEN(single_eq)
"&"                                     RET_TOKEN(bitwise_and)
"|"                                     RET_TOKEN(bitwise_or)
"~"                                     RET_TOKEN(bitwise_not)
"^"                                     RET_TOKEN(bitwise_xor)
"&&"                                    RET_TOKEN(logical_and)
"||"                                    RET_TOKEN(logical_or)
"!"                                     RET_TOKEN(logical_not)
"@"                                     RET_TOKEN(at)
"//"                                    RET_TOKEN(single_line_comment)
"/*"                                    RET_TOKEN(multi_line_comment_start)
"*/"                                    RET_TOKEN(multi_line_comment_end)
[\ \t\r]+                               { yycolumn = yycolumn_last; yycolumn_last += yyleng; }
[\n]                                    { yy_lineno++; yycolumn = 1; yycolumn_last = 1; return TOKEN_single_line_comment_end; }
.                                       { yycolumn = yycolumn_last; yycolumn_last += yyleng; }
%%

void yyerror(const char *s) {
    fprintf(stderr, "Error: %s\n", s);
}
