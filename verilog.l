%{
    #include <stdio.h>
    #include <string.h>
    #include "Lexer.h"
    #define RET_KEYWORD(token)      {   unput(yytext[yyleng - 1]); \
                                        char *thistok = strdup(yytext); \
                                        thistok[yyleng - 1] = 0; \
                                        yylval.str = thistok; \
                                        return TOKEN_##token; \
                                    }
    #define RET_TOKEN(name)         { yylval.str = strdup(yytext); return TOKEN_##name; }

    void yyerror(const char *s);
%}

%%

module[^A-Za-z0-9]                      RET_KEYWORD(module)
endmodule[^A-Za-z0-9]                   RET_KEYWORD(endmodule)
output[^A-Za-z0-9]                      RET_KEYWORD(output)
input[^A-Za-z0-9]                       RET_KEYWORD(input)
assign[^A-Za-z0-9]                      RET_KEYWORD(assign)
wire[^A-Za-z0-9]                        RET_KEYWORD(wire)
begin[^A-Za-z0-9]                       RET_KEYWORD(begin)
end[^A-Za-z0-9]                         RET_KEYWORD(end)
case[^A-Za-z0-9]                        RET_KEYWORD(case)
casez[^A-Za-z0-9]                       RET_KEYWORD(casez)
casex[^A-Za-z0-9]                       RET_KEYWORD(casex)
endcase[^A-Za-z0-9]                     RET_KEYWORD(endcase)
if[^A-Za-z0-9]                          RET_KEYWORD(if)
posedge[^A-Za-z0-9]                     RET_KEYWORD(posedge)
reg[^A-Za-z0-9]                         RET_KEYWORD(reg)
","                                     RET_TOKEN(comma)
";"                                     RET_TOKEN(semicolon)
":"                                     RET_TOKEN(colon)
[A-Za-z][A-Za-z0-9]*                    RET_TOKEN(identifier)
\-?[0-9]+                               RET_TOKEN(number)
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
"=="                                    RET_TOKEN(cond_eq)
">"                                     RET_TOKEN(cond_gt)
"<"                                     RET_TOKEN(cond_lt)
"="                                     RET_TOKEN(single_eq)
"&"                                     RET_TOKEN(bitwise_and)
"|"                                     RET_TOKEN(bitwise_or)
"~"                                     RET_TOKEN(bitwise_not)
"^"                                     RET_TOKEN(bitwise_xor)
"&&"                                    RET_TOKEN(logical_and)
"||"                                    RET_TOKEN(logical_or)
"!"                                     RET_TOKEN(logical_not)
"//"                                    RET_TOKEN(single_line_comment)
"/*"                                    RET_TOKEN(multi_line_comment_start)
"*/"                                    RET_TOKEN(multi_line_comment_end)
[\ \t\r\n]+                             {}

%%

void yyerror(const char *s) {
    fprintf(stderr, "Error: %s\n", s);
}
