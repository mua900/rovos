#include "lang.hpp"
#include "parse.hpp"
#include "token.hpp"
#include "statement.hpp"
#include "bytecode.hpp"

static const char* interpLastError = nullptr;

struct Interp {
    ProgramTree tree = {};
    Bytecode_Program program = {};
    String_Builder buffer = {};
};

const char* interp_get_last_error() { return interpLastError; }
Interp* interp_create() {
    Interp* interp = new Interp;
    return interp;
}

void interp_destroy(Interp* interp) {
    delete interp;
}

Interp* interp_copy(Interp* interp) {
    Interp* intr = new Interp;
    intr->tree = interp->tree;
    intr->program = interp->program;
    intr->buffer = interp->buffer;
    return intr;
}

bool interp_syntax_check(const char* program_string, int length) {
    Parser parser = Parser();
    String program = String(program_string, length);

    return parser.syntax_check(program);
}

bool interp_set_program(Interp* interp, const char* program_string, int length) {
    Parser parser = Parser();
    String program = String(program_string, length);
    ProgramTree tree = {};
    if (!parser.parse(program, tree)) {
        return false;
    }

    Bytecode_Program bytecode = {};
    
    if (!bytecode_compile_program(bytecode, tree)) {
        return false;
    }
    interp->tree = tree;
    interp->program = bytecode;
    return true;
}

void interp_run_program(Interp* interp) {
    bytecode_run(interp->program);
}

int interp_register_variable(Interp* interp, const char* name, int length, String type) {
    String symbol = String(name, length);
    symbol = interp->buffer.put_string(symbol);
    Variable var = Variable(symbol, type);

    for (int i = 0; i < interp->program.variables.size(); i++) {
        String var_name = interp->program.variables.get(i).variable.name;
        if (string_compare(var_name, symbol)) {
            interpLastError = "Trying to register the same variable name more than once.";
            return i;
        }
    }

    int var_id = interp->program.add_symbol(var);
    return var_id;
}

bool interp_set_variable_value(Interp* interp, int variable, Value value) {
    if (!interp->program.variables.in_bounds(variable)) {
        interpLastError = "Trying to set a variable out of bounds";
        return false;
    }

    interp->program.variables.get_ref(variable).value = value;
    return true;
}

Value interp_get_variable_value(const Interp* interp, int variable) {
    if (!interp->program.variables.in_bounds(variable)) {
        interpLastError = "Trying to get a variable out of bounds";
        return Value();
    }

    return interp->program.variables.get(variable).value;
}

InterpString interp_get_variable_name(const Interp* interp, int variable) {
    InterpString name = {};
    if (!interp->program.variables.in_bounds(variable)) {
        interpLastError = "Trying to get a variable's name out of bounds";
        return name;
    }

    String n = interp->program.variables.get(variable).variable.name;
    name.data = n.data;
    name.size = n.size;
    return name;
}

int interp_get_variable_count(const Interp* interp) {
    return interp->program.variables.size();
}

int interp_register_function(Interp* interp, const char* name, int length, Variable_Type type) {
    // @todo
    ASSERT(false);
    return 0;
}

bool interp_set_function_callback(Interp* interp, int variable, LangFunction callback) {
    // @todo
    ASSERT(false);
    return 0;
}

LangFunction interp_get_function_callback(const Interp* interp, int variable) {
    // @todo
    ASSERT(false);
    return nullptr;
}

const char* interp_get_function_name_at_index(const Interp* interp, int index) {
    // @todo
    ASSERT(false);
    return nullptr;
}

int interp_get_function_count(const Interp* interp) {
    // @todo
    ASSERT(false);
    return 0;
}

void interp_set_input_stream(Interp* interp, float* input_stream, int input_stream_size, int stride) {
    interp->program.set_input_stream(InputStream(Array<float>(input_stream, input_stream_size), stride));
}

void interp_clear_input_stream(Interp* interp) {
    interp->program.input_stream = InputStream();
}

bool Parser::parse(String program, ProgramTree& tree) {
    return false;
}

Statement* Parser::parse_statement() {
    bool builtin = false;

    switch (tokens.get(cursor).type) {
        case TOKEN_TYPE_IF: {
            return parse_if();
        }
        case TOKEN_TYPE_FOR: {
            return parse_for();
        }
        case TOKEN_TYPE_WHILE: {
            return parse_while();
        }
        case TOKEN_TYPE_BUILTIN: {
            ASSERT(consume(TOKEN_TYPE_BUILTIN));
            builtin = true;
        }  // fallthrough
        case TOKEN_TYPE_PROC: {
            return parse_procedure_declaration(builtin);
        }
        case TOKEN_TYPE_TYPE: {
            return parse_type_declaration();
        }
        case TOKEN_TYPE_BRACE_OPEN: {
            return parse_block_statement();
        }
        case TOKEN_TYPE_VAR: {
            return parse_variable_declaration();
        }
        default: {
            Expr* expr = parse_expression();
            if (tokens.get(cursor).type == TOKEN_TYPE_EQUALS) {
                cursor ++;
                Expr* rhs = parse_expression();
                if (!consume(TOKEN_TYPE_SEMICOLON)) {
                    parser_error = Error("Missing ; at the end of assignment", tokens.get(cursor).offset);
                }

                return new StmtAssignment(expr, rhs);
            }
            else {
                return new StmtExpression(expr);
            }
        }
    }
}

StmtBlock* Parser::parse_block_statement() {
    if (!consume(TOKEN_TYPE_BRACE_OPEN)) {
        return nullptr;
    }

    DArray<Statement*> body;
    while (cursor < tokens.size && tokens.get(cursor).type != TOKEN_TYPE_BRACE_CLOSE)
    {
        Statement* stmt = parse_statement();
        body.add(stmt);
    }

    if (tokens.get(cursor).type != TOKEN_TYPE_BRACE_CLOSE)
    {
        return nullptr;
    }

    cursor ++;

    return new StmtBlock(body);
}

StmtDeclVar* Parser::parse_variable_declaration() {
    if (!consume(TOKEN_TYPE_VAR)) { return nullptr; }

    if (tokens.get(cursor).type != TOKEN_TYPE_IDENT)
    {
        parser_error = Error("Expected variable name after var keyword", tokens.get(cursor).offset);
        return nullptr;
    }

    String name = tokens.get(cursor).token_string;
    cursor ++;

    bool infer_type = false;
    String type_name = {};
    if (tokens.get(cursor).type == TOKEN_TYPE_WALRUS) {
        infer_type = true;
        cursor ++;
    }
    else if (tokens.get(cursor).type == TOKEN_TYPE_COLON) {
        cursor ++;
        if (tokens.get(cursor).type != TOKEN_TYPE_IDENT) {
            return nullptr;
        }
        type_name = tokens.get(cursor).token_string;
        cursor ++;
        if (!consume(TOKEN_TYPE_EQUALS)) {
            return nullptr;
        }
    }
    else {
        return nullptr;
    }

    Expr* rhs = parse_expression();
    if (!rhs)
    {
        return nullptr;
    }

    return new StmtDeclVar(name, rhs, type_name, infer_type);
}

StmtDeclProc* Parser::parse_procedure_declaration(bool builtin) {
    if (!consume(TOKEN_TYPE_PROC)) { return nullptr; }

    if (tokens.get(cursor).type == TOKEN_TYPE_IDENT)
    {
        parser_error = Error("Expeceted procedure name after proc keyword", tokens.get(cursor).offset);
        return nullptr;
    }

    String name = tokens.get(cursor).token_string;
    cursor ++;

    ProcFlags flags = builtin ? PROC_IS_BUILTIN : 0;

    DArray<Parameter> params = {};

    if (!consume(TOKEN_TYPE_PAREN_OPEN)) { return nullptr; }
    while (cursor < tokens.size && tokens.get(cursor).type != TOKEN_TYPE_PAREN_CLOSE)
    {
        ParameterFlags paramFlags = 0;

        if (tokens.get(cursor).type == TOKEN_TYPE_CONST) {
            paramFlags |= PARAMETER_IS_CONST;
            cursor ++;
        }
        if (tokens.get(cursor).type == TOKEN_TYPE_IN) {
            paramFlags |= PARAMETER_IS_INPUT;
            cursor ++;
        }
        if (tokens.get(cursor).type == TOKEN_TYPE_OUT) {
            paramFlags |= PARAMETER_IS_OUTPUT;
            cursor ++;
        }

        if (!(paramFlags & PARAMETER_IS_INPUT) && !(paramFlags & PARAMETER_IS_OUTPUT)) {
            paramFlags |= PARAMETER_IS_INPUT & PARAMETER_IS_OUTPUT;
        }

        if (tokens.get(cursor).type != TOKEN_TYPE_IDENT) {
            return nullptr;
        }
        String param_name = tokens.get(cursor).token_string;

        if (!consume(TOKEN_TYPE_COLON)) {
            return nullptr;
        }

        Token type_token = tokens.get(cursor);
        if (!is_type_token(type_token.type)) {
            return nullptr;
        }

        if (tokens.get(cursor).type == TOKEN_TYPE_PAREN_CLOSE)
            break;

        if (!consume(TOKEN_TYPE_COMMA)) {
            return nullptr;
        }

        params.add(Parameter(param_name, type_token.token_string, paramFlags));
    }
    if (!consume(TOKEN_TYPE_PAREN_CLOSE)) { return nullptr; }

    if (!consume(TOKEN_TYPE_BRACE_OPEN)) { return nullptr; }

    DArray<Statement*> body;
    while (cursor < tokens.size && tokens.get(cursor).type != TOKEN_TYPE_BRACE_CLOSE) {
        Statement* stmt = parse_statement();
        if (!stmt) {
            return nullptr;
        }

        body.add(stmt);
    }

    if (!consume(TOKEN_TYPE_BRACE_CLOSE)) { return nullptr; }

    return new StmtDeclProc(name, params, body, flags);
}

StmtDeclType* Parser::parse_type_declaration() {
    if (!consume(TOKEN_TYPE_TYPE)) { return nullptr; }

    if (tokens.get(cursor).type != TOKEN_TYPE_IDENT) {
        return nullptr;
    }

    String name = tokens.get(cursor).token_string;

    TypeFlags flags = 0;

    auto kind = tokens.get(cursor).type;
    if (kind == TOKEN_TYPE_STRUCT) {
        flags |= TypeIsComposite;
    }
    else if (kind == TOKEN_TYPE_ENUM) {
        flags |= TypeIsEnum;
    }

    if (!consume(TOKEN_TYPE_BRACE_OPEN)) {
        return nullptr;
    }

    DArray<Variable> field;
    while (cursor < tokens.size && tokens.get(cursor).type != TOKEN_TYPE_BRACE_CLOSE) {
        if (tokens.get(cursor).type != TOKEN_TYPE_IDENT) {
            return nullptr;
        }

        Variable variable = {};
        variable.name = tokens.get(cursor).token_string;
        cursor ++;
        
        if (!consume(TOKEN_TYPE_COLON)) {
            return nullptr;
        }

        if (!is_type_token(tokens.get(cursor).type)) {
            return nullptr;
        }

        variable.type_name = tokens.get(cursor).token_string;
        cursor ++;

        field.add(variable);
    }

    if (!consume(TOKEN_TYPE_BRACE_CLOSE)) {
        return nullptr;
    }

    return new StmtDeclType(name, field, flags);
}

StmtIf* Parser::parse_if() {
    if (!consume(TOKEN_TYPE_IF)) {
        return nullptr;
    }

    Expr* condition = parse_expression();
    if (!condition) {
        return nullptr;
    }

    if (!consume(TOKEN_TYPE_BRACE_OPEN)) {
        return nullptr;
    }

    Statement* then_ = parse_statement();
    if (!then_) return nullptr;
    Statement* else_ = nullptr;
    if (tokens.get(cursor).type == TOKEN_TYPE_ELSE) {
        cursor ++;
        else_ = parse_statement();
        if (!else_) return nullptr;
    }

    return new StmtIf(condition, then_, else_);
}

StmtFor* Parser::parse_for() {
    if (!consume(TOKEN_TYPE_FOR)) {
        return nullptr;
    }

    Expr* start = parse_expression();
    if (!start) return nullptr;
    if (!consume(TOKEN_TYPE_SEMICOLON)) return nullptr;
    Expr* condition = parse_expression();
    if (!condition) return nullptr;
    if (!consume(TOKEN_TYPE_SEMICOLON)) return nullptr;
    Expr* end = parse_expression();
    if (!end) return nullptr;

    Statement* body = parse_statement();

    return new StmtFor(start, condition, end, body);
}

StmtWhile* Parser::parse_while() {
    if (!consume(TOKEN_TYPE_WHILE)) return nullptr;
    Expr* condition = parse_expression();
    if (!condition) return nullptr;

    Statement* body = parse_statement();
    return new StmtWhile(condition, body);
}


bool is_type_token(Token_Type type)
{
    return type == TOKEN_TYPE_INT || type == TOKEN_TYPE_FLOAT || type == TOKEN_TYPE_BOOL
        || type == TOKEN_TYPE_IDENT;
}

bool Parser::consume(Token_Type type)
{
    bool match = (tokens.get(cursor).type == type);
    if (match)
    {
        cursor++;
    }
    return match;
}

bool Value::evaluate_truth_value() {
    switch (type) {
        case Var_Type_Boolean: return boolean;
        case Var_Type_Integer: return integer != 0;
        case Var_Type_Real: return real != 0.0;
    }

    panic("Unknown value type");
}

DArray<Token> tokenize(String expression)
{
    auto tokens = DArray<Token>(8);

#define ADD_TOKEN(p_token_type, p_token_string) tokens.add(Token(make_string(p_token_string), p_token_type, cursor)); cursor++;

    int cursor = 0;
    while (cursor < expression.size)
    {
        char ch = expression.data[cursor];

        switch (ch)
        {
            case '+':
                ADD_TOKEN(TOKEN_TYPE_PLUS, "+")
                break;
            case '-':
                ADD_TOKEN(TOKEN_TYPE_MINUS, "-")
                break;
            case '*':
                ADD_TOKEN(TOKEN_TYPE_STAR, "*")
                break;
            case '/':
                ADD_TOKEN(TOKEN_TYPE_SLASH, "/")
                break;
            case '%':
                ADD_TOKEN(TOKEN_TYPE_PERCENT, "%")
                break;
            case ',':
                ADD_TOKEN(TOKEN_TYPE_COMMA, ",")
                break;
            case ':':
                ADD_TOKEN(TOKEN_TYPE_COLON, ":")
                break;
            case ';':
                ADD_TOKEN(TOKEN_TYPE_SEMICOLON, ";")
                break;
            case '&':
                ADD_TOKEN(TOKEN_TYPE_AMPERSAND, "&")
                break;
            case '(':
                ADD_TOKEN(TOKEN_TYPE_PAREN_OPEN, "(")
                break;
            case ')':
                ADD_TOKEN(TOKEN_TYPE_PAREN_CLOSE, ")")
                break;
            case '{':
                ADD_TOKEN(TOKEN_TYPE_BRACE_OPEN, "{")
                break;
            case '}':
                ADD_TOKEN(TOKEN_TYPE_BRACE_CLOSE, "}")
                break;
            case '?':
				ADD_TOKEN(TOKEN_TYPE_QUESTION_MARK, "?")
				break;
            case '!': {
                if (expression.data[cursor+1] == '=')
                {
                    ADD_TOKEN(TOKEN_TYPE_EXCLAMATION_EQUALS, "!=")
                }
                else
                {
                    ADD_TOKEN(TOKEN_TYPE_EXCLAMATION, "!")
                }
                break;
            }
            case '=': {
                if (expression.data[cursor+1] == '=')
                {
                    ADD_TOKEN(TOKEN_TYPE_EQUALS_EQUALS, "==")
                }
                else
                {
                    ADD_TOKEN(TOKEN_TYPE_EQUALS, "=")
                }
                break;
            }
            case '>': {
                if (expression.data[cursor+1] == '=')
                {
                    ADD_TOKEN(TOKEN_TYPE_GREATER_EQUALS, ">=");
                }
                else
                {
                    ADD_TOKEN(TOKEN_TYPE_GREATER, ">")
                }
                break;
            }
            case '<': {
                if (expression.data[cursor+1] == '=')
                {
                    ADD_TOKEN(TOKEN_TYPE_LESS_EQUALS, "<=");
                }
                else
                {
                    ADD_TOKEN(TOKEN_TYPE_LESS, "<");
                }
                break;
            }
            default:
            {
                if (is_space(ch))
                {
                    while (is_space(ch))
                    {
                        ch = expression.data[cursor];
                        cursor++;
                    }
                    continue;
                }

                const auto is_ident_character = [](char ch){return is_alpha(ch) || ch == '_';};
                if (is_ident_character(ch))
                {
                    int start = cursor;
                    while (is_ident_character(ch) || is_digit(ch))
                    {
                        cursor++;
                        ch = expression.data[cursor];
                    }

                    String ident = string_slice(expression, start, cursor);
                    String builtin_ = String("builtin");
                    String proc_ = String("proc");
                    String if_ = String("if");
                    String else_ = String("else");
                    String for_ = String("for");
                    String while_ = String("while");
                    String var_ = String("var");
                    String type_ = String("type");
                    String struct_ = String("struct");
                    String enum_ = String("enum");
                    String int_ = String("int");
                    String float_ = String("float");
                    String bool_ = String("bool");

                    if (string_compare(builtin_, ident)) {
                        tokens.add(Token(builtin_, TOKEN_TYPE_BUILTIN, cursor));
                    }
                    else if (string_compare(proc_, ident)) {
                        tokens.add(Token(proc_, TOKEN_TYPE_PROC, cursor));
                    }
                    else if (string_compare(if_, ident)) {
                        tokens.add(Token(if_, TOKEN_TYPE_IF, cursor));
                    }
                    else if (string_compare(else_, ident)) {
                        tokens.add(Token(else_, TOKEN_TYPE_ELSE, cursor));
                    }
                    else if (string_compare(for_, ident)) {
                        tokens.add(Token(for_, TOKEN_TYPE_FOR, cursor));
                    }
                    else if (string_compare(while_, ident)) {
                        tokens.add(Token(while_, TOKEN_TYPE_WHILE, cursor));
                    }
                    else if (string_compare(var_, ident)) {
                        tokens.add(Token(var_, TOKEN_TYPE_VAR, cursor));
                    }
                    else if (string_compare(type_, ident)) {
                        tokens.add(Token(type_, TOKEN_TYPE_TYPE, cursor));
                    }
                    else if (string_compare(struct_, ident)) {
                        tokens.add(Token(struct_, TOKEN_TYPE_STRUCT, cursor));
                    }
                    else if (string_compare(enum_, ident)) {
                        tokens.add(Token(enum_, TOKEN_TYPE_ENUM, cursor));
                    }
                    else if (string_compare(int_, ident)) {
                        tokens.add(Token(int_, TOKEN_TYPE_INT, cursor));
                    }
                    else if (string_compare(float_, ident)) {
                        tokens.add(Token(float_, TOKEN_TYPE_FLOAT, cursor));
                    }
                    else if (string_compare(bool_, ident)) {
                        tokens.add(Token(bool_, TOKEN_TYPE_BOOL, cursor));
                    }
                    else {
                        tokens.add(Token(ident, TOKEN_TYPE_IDENT, cursor));
                    }
                }
                else if (is_digit(ch))
                {
                    int start = cursor;
                    while (is_digit(ch)) { cursor++; ch = expression.data[cursor]; }

                    if (ch == '.')
                    {
                        cursor++;
                        ch = expression.data[cursor];
                        while (is_digit(ch)) { cursor++; ch = expression.data[cursor]; }
                        String ident = string_slice(expression, start, cursor);
                        tokens.add(Token(ident, TOKEN_TYPE_LITERAL_FLOAT, cursor));
                    }
                    else
                    {
                        String ident = string_slice(expression, start, cursor);
                        tokens.add(Token(ident, TOKEN_TYPE_LITERAL_INT, cursor));
                    }
                }
                else if (ch == '"')
                {
                    int start = cursor + 1;
                    cursor++;
                    while (ch != '"')
                    {
                        if (cursor >= expression.size)
                        {
                            fprintf(stderr, "Unterminated string literal\n");
                            break;
                        }
                        cursor++;
                        ch = expression.data[cursor];
                    }

                    String s_literal = string_slice(expression, start, cursor);
                    tokens.add(Token(s_literal, TOKEN_TYPE_LITERAL_STRING, start));
                }
                else {
                    fprintf(stderr, "Unknown character %c", ch);
                    cursor++;
                }
            }
        }
    }

    tokens.add(Token(make_string("END"), TOKEN_TYPE_END, cursor));
    tokens.add(Token(make_string("END"), TOKEN_TYPE_END, cursor));

    return tokens;
}
