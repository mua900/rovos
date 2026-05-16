#ifndef _PARSE_H
#define _PARSE_H

#include "expr.hpp"
#include "statement.hpp"
#include "lang_common.hpp"

struct Error {
    const char* message;
    int offset = 0;

    Error() {}
    Error(const char* msg, int off) : message(msg), offset(off) {}
};

struct Parser {
    Parser() {}

    bool parse(String program, ProgramTree& tree);

    void set_symbols(Array<Variable> p_symbols) { symbols = p_symbols; }

    bool syntax_check(String program);
    bool check_program(String program);

    Error get_error() const { return parser_error; }
    void clear_error() { parser_error = Error(); }

private:
    Array<Variable> symbols = {};
    Array<Token> tokens;

    int cursor = 0;
    Error parser_error = {};

    Statement* parse_statement();
    StmtBlock* parse_block_statement();
    StmtDeclVar* parse_variable_declaration();
    StmtDeclProc* parse_procedure_declaration(bool builtin);
    StmtDeclType* parse_type_declaration();
    // StmtAssignment* parse_assignment();
    StmtIf* parse_if();
    StmtFor* parse_for();
    StmtWhile* parse_while();
    StmtExpression* parse_expression_statement();

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

bool is_type_token(Token_Type type);

#endif // _PARSE_H
