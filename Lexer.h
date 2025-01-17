#ifndef VERIPYTHON_LEXER__
#define VERIPYTHON_LEXER__

enum VeriPythonTokens {
    TOKEN_module = -1024,
    TOKEN_endmodule,
    TOKEN_output,
    TOKEN_input,
    TOKEN_assign,
    TOKEN_wire,
    TOKEN_always,
    TOKEN_posedge,
    TOKEN_negedge,
    TOKEN_senslist_or,
    TOKEN_begin,
    TOKEN_end,
    TOKEN_case,
    TOKEN_casez,
    TOKEN_casex,
    TOKEN_endcase,
    TOKEN_if,
    TOKEN_else,
    TOKEN_reg,
    TOKEN_comma,
    TOKEN_semicolon,
    TOKEN_colon,
    TOKEN_question,
    TOKEN_identifier,
    TOKEN_const_number,
    TOKEN_sized_number,
    TOKEN_lparen,
    TOKEN_rparen,
    TOKEN_lbrace,
    TOKEN_rbrace,
    TOKEN_lbracket,
    TOKEN_rbracket,
    TOKEN_op_add,
    TOKEN_op_sub,
    TOKEN_op_mul,
    TOKEN_op_div,
    TOKEN_op_mod,
    TOKEN_arith_lshift,
    TOKEN_logical_lshift,
    TOKEN_arith_rshift,
    TOKEN_logical_rshift,
    TOKEN_cond_eq,
    TOKEN_cond_ne,
    TOKEN_cond_ge,
    TOKEN_cond_gt,
    TOKEN_cond_le,
    TOKEN_cond_lt,
    TOKEN_single_eq,
    TOKEN_bitwise_and,
    TOKEN_bitwise_or,
    TOKEN_bitwise_not,
    TOKEN_bitwise_xor,
    TOKEN_logical_and,
    TOKEN_logical_or,
    TOKEN_logical_not,
    TOKEN_at,
    TOKEN_single_line_comment,
    TOKEN_single_line_comment_end,
    TOKEN_multi_line_comment_start,
    TOKEN_multi_line_comment_end,
};

union LEXER_VALUE {
    /* 需要手动释放 */
    char *str;

};

extern union LEXER_VALUE yylval;

#endif
