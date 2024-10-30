%{
    #include <stdio.h>
    #include <string.h>
    #define MAKE_UP_KEYWORD(name)   {   unput(yytext[yyleng - 1]); \
                                        char *thistok = strdup(yytext); \
                                        thistok[yyleng - 1] = 0; \
                                        printf("<'" #name "', '%s'>\n", thistok); \
                                        free(thistok); \
                                    }
    #define MAKE_UP_TOKEN(name)     printf("<'" #name "', '%s'>\n", yytext)

    void yyerror(const char *s);
%}

%%

module[^A-Za-z0-9]                   MAKE_UP_KEYWORD(module)
endmodule[^A-Za-z0-9]                     MAKE_UP_KEYWORD(endmodule)
output[^A-Za-z0-9]                   MAKE_UP_KEYWORD(output)
input[^A-Za-z0-9]                     MAKE_UP_KEYWORD(input)
assign[^A-Za-z0-9]                     MAKE_UP_KEYWORD(assign)
wire[^A-Za-z0-9]                     MAKE_UP_KEYWORD(wire)
begin[^A-Za-z0-9]                     MAKE_UP_KEYWORD(begin)
end[^A-Za-z0-9]                     MAKE_UP_KEYWORD(end)
case[^A-Za-z0-9]                     MAKE_UP_KEYWORD(case)
casez[^A-Za-z0-9]                     MAKE_UP_KEYWORD(casez)
casex[^A-Za-z0-9]                     MAKE_UP_KEYWORD(casex)
endcase[^A-Za-z0-9]                     MAKE_UP_KEYWORD(endcase)
if[^A-Za-z0-9]                      MAKE_UP_KEYWORD(if)
posedge[^A-Za-z0-9]                   MAKE_UP_KEYWORD(posedge)
reg[^A-Za-z0-9]                   MAKE_UP_KEYWORD(reg)
","                                 { MAKE_UP_TOKEN(comma); }
";"                                 { MAKE_UP_TOKEN(semicolon); }
":"                                 { MAKE_UP_TOKEN(colon); }
[A-Za-z][A-Za-z0-9]*                { MAKE_UP_TOKEN(identifier); }
\-?[0-9]+                           { MAKE_UP_TOKEN(number); }
"("                                 { MAKE_UP_TOKEN(lparen); }
")"                                 { MAKE_UP_TOKEN(rparen); }
"{"                                 { MAKE_UP_TOKEN(lbrace); }
"}"                                 { MAKE_UP_TOKEN(rbrace); }
"["                                 { MAKE_UP_TOKEN(lbracket); }
"]"                                 { MAKE_UP_TOKEN(rbracket); }
"+"                                 { MAKE_UP_TOKEN(op_add); }
"-"                                 { MAKE_UP_TOKEN(op_sub); }
"*"                                 { MAKE_UP_TOKEN(op_mul); }
"/"                                 { MAKE_UP_TOKEN(op_div); }
"=="                                { MAKE_UP_TOKEN(cond_eq); }
">"                                 { MAKE_UP_TOKEN(cond_gt); }
"<"                                 { MAKE_UP_TOKEN(cond_lt); }
"="                                 { MAKE_UP_TOKEN(single_eq); }
"&"                                 { MAKE_UP_TOKEN(bitwise_and); }
"|"                                 { MAKE_UP_TOKEN(bitwise_or); }
"~"                                 { MAKE_UP_TOKEN(bitwise_not); }
"^"                                 { MAKE_UP_TOKEN(bitwise_xor); }
"&&"                                 { MAKE_UP_TOKEN(logical_and); }
"||"                                 { MAKE_UP_TOKEN(logical_or); }
"!"                                 { MAKE_UP_TOKEN(logical_not); }
"//"                                 { MAKE_UP_TOKEN(single_line_comment); }
"/*"                                 { MAKE_UP_TOKEN(multiline_comment_start); }
"*/"                                 { MAKE_UP_TOKEN(multiline_comment_end); }
[\ \t\r\n]+                         {}

%%

int main(int argc, char **argv) {
    yylex();
    return 0;
}

void yyerror(const char *s) {
    fprintf(stderr, "Error: %s\n", s);
}