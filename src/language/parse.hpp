#ifndef _PARSE_H
#define _PARSE_H

#include "expr.hpp"

struct Error {
    const char* message;
    int offset = 0;

    Error() {}
    Error(const char* msg, int off) : message(msg), offset(off) {}
};

struct Parser {
    Parser() {}

    Expr* parse(String expression);

    void set_symbols(Array<Variable> p_symbols) { symbols = p_symbols; }

    bool syntax_check(String expression);
    bool check_expression_string(String expression);

    Error get_error() const { return parser_error; }
    void clear_error() { parser_error = Error(); }

private:
    Array<Variable> symbols = {};
    Array<Token> tokens;

    int cursor = 0;
    Error parser_error = {};

    Expr* parse_expression();

    // according to precedence in order
	Expr* parse_ternary_expr();
    Expr* parse_equality_expr();
    Expr* parse_comparison_expr();
    Expr* parse_arithmetic_expr();
    Expr* parse_factor_expr();
	Expr* parse_mod_expr();
    Expr* parse_unary_expr();
    Expr* parse_call_expr();
    Expr* parse_primary_expr();

    bool consume(Token_Type type);
};

#endif // _PARSE_H
