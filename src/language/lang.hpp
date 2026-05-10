#ifndef _LANG_H
#define _LANG_H

#include "common.hpp"

extern "C" {

    enum Variable_Type : int {
        Var_Type_Integer,
        Var_Type_Real,
        Var_Type_Boolean,
    };
    
    inline bool is_numeric(Variable_Type type) { return type == Var_Type_Integer || type == Var_Type_Real; }

    struct Value {
        Variable_Type type;

        union {
            bool boolean;
            s64 integer;
            double real;
        };

        Value() : type(Var_Type_Integer), integer(0) {}  // invalid state
        Value(bool b) : type(Var_Type_Boolean), boolean(b) {}
        Value(long long integer) : type(Var_Type_Integer), integer(integer) {}
        Value(double real) : type(Var_Type_Real), real(real) {}

        bool evaluate_truth_value();
    };

    struct Interp;

    typedef void* (*LangFunction)(void*);

    const char* interp_get_last_error();
    Interp* interp_create();
    void interp_destroy(Interp* interp);
    Interp* interp_copy(Interp* interp);
    void interp_reset(Interp* interp);
    bool interp_check_program_string(const Interp* interp_or_null, const char* program_string, int length);
    bool interp_set_program(Interp* interp, const char* program_string, int length);
    void interp_run_program(Interp* interp);
    int interp_register_variable(Interp* interp, const char* name, int length, Variable_Type type);
    bool interp_set_variable_value(Interp* interp, int variable, Value value);
    Value interp_get_variable_value(const Interp* interp, int variable);
    const char* interp_get_variable_name_at_index(const Interp* interp, int index);
    int interp_get_variable_count(const Interp* interp);
    int interp_register_function(Interp* interp, const char* name, int length, Variable_Type type);
    bool interp_set_function_callback(Interp* interp, int variable, LangFunction callback);
    LangFunction interp_get_function_callback(const Interp* interp, int variable);
    const char* interp_get_function_name_at_index(const Interp* interp, int index);
    int interp_get_function_count(const Interp* interp);
    void interp_set_input_stream(Interp* interp, float* input_stream, int input_stream_size, int stride);
    void interp_clear_input_stream(Interp* interp);
}

#endif // _LANG_H
