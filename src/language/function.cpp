#include "function.hpp"

void call_function(Function func, const Value* parameters, Value* results) {
    func.implementation(parameters, results);
}

double fract(double x) {
    return x - floor(x);
}

double get_sign(double x)
{
    return (x > 0) - (x < 0);
}

double smoothstep(double x) {
    x = CLAMP(x, 0.0, 1.0);
    return x * x * (3.0 - 2.0 * x);
}

double clamp(double x, double lower, double upper) {
    return CLAMP(x, lower, upper);
}

double mix(double a, double b, double t) {
    return (1.0 - t) * a + t * b;
}

double saw(double x) {
    return 2 * (x - floor(x + 0.5));
}

double square(double x) {
    return floor(x) * 4 - floor(2 * x) * 2 + 1;
}

double triangle(double x) {
    return 4 * abs(x - floor(x + 0.75) + 0.25) - 1;  // might be wrong?
}

// wrappers

void interp_fabs(Value* parameters, Value* results) {
    Value val;
    val.real = fabs(parameters[0].real);
    val.type = Var_Type_Real;
    results[0] = val;
}
void interp_get_sign(Value* parameters, Value* results) {
    Value val;
    val.real = get_sign(parameters[0].real);
    val.type = Var_Type_Real;
    results[0] = val;
}
void interp_ceil(Value* parameters, Value* results) {
    Value val;
    val.real = ceil(parameters[0].real);
    val.type = Var_Type_Real;
    results[0] = val;
}
void interp_floor(Value* parameters, Value* results) {
    Value val;
    val.real = floor(parameters[0].real);
    val.type = Var_Type_Real;
    results[0] = val;
}
void interp_sin(Value* parameters, Value* results) {
    Value val;
    val.real = sin(parameters[0].real);
    val.type = Var_Type_Real;
    results[0] = val;
}
void interp_cos(Value* parameters, Value* results) {
    Value val;
    val.real = cos(parameters[0].real);
    val.type = Var_Type_Real;
    results[0] = val;
}
void interp_tan(Value* parameters, Value* results) {
    Value val;
    val.real = tan(parameters[0].real);
    val.type = Var_Type_Real;
    results[0] = val;
}
void interp_asin(Value* parameters, Value* results) {
    Value val;
    val.real = asin(parameters[0].real);
    val.type = Var_Type_Real;
    results[0] = val;
}
void interp_acos(Value* parameters, Value* results) {
    Value val;
    val.real = acos(parameters[0].real);
    val.type = Var_Type_Real;
    results[0] = val;
}
void interp_atan(Value* parameters, Value* results) {
    Value val;
    val.real = atan(parameters[0].real);
    val.type = Var_Type_Real;
    results[0] = val;
}
void interp_exp(Value* parameters, Value* results) {
    Value val;
    val.real = exp(parameters[0].real);
    val.type = Var_Type_Real;
    results[0] = val;
}
void interp_log(Value* parameters, Value* results) {
    Value val;
    val.real = log(parameters[0].real);
    val.type = Var_Type_Real;
    results[0] = val;
}
void interp_smoothstep(Value* parameters, Value* results) {
    Value val;
    val.real = smoothstep(parameters[0].real);
    val.type = Var_Type_Real;
    results[0] = val;
}
void interp_clamp(Value* parameters, Value* results) {
    Value val;
    val.type = Var_Type_Real;
    val.real = clamp(parameters[0].real, parameters[1].real, parameters[2].real);
    results[0] = val;
}
void interp_pow(Value* parameters, Value* results) {
    Value val;
    val.type = Var_Type_Real;
    val.real = pow(parameters[0].real, parameters[1].real);
    results[0] = val;
}
void interp_fract(Value* parameters, Value* results) {
    Value val;
    val.type = Var_Type_Real;
    val.real = fract(parameters[0].real);
    results[0] = val;
}
void interp_mix(Value* parameters, Value* results) {
    Value val;
    val.type = Var_Type_Real;
    val.real = mix(parameters[0].real, parameters[1].real, parameters[2].real);
    results[0] = val;
}
void interp_saw(Value* parameters, Value* results) {
    Value val;
    val.type = Var_Type_Real;
    val.real = saw(parameters[0].real);
    results[0] = val;
}
void interp_square(Value* parameters, Value* results) {
    Value val;
    val.type = Var_Type_Real;
    val.real = square(parameters[0].real);
    results[0] = val;
}
void interp_triangle(Value* parameters, Value* results) {
    Value val;
    val.type = Var_Type_Real;
    val.real = triangle(parameters[0].real);
    results[0] = val;
}
void interp_min(Value* parameters, Value* results) {
    Value val;
    val.type = Var_Type_Real;
    val.real = std::min(parameters[0].real, parameters[1].real);
    results[0] = val;
}
void interp_max(Value* parameters, Value* results) {
    Value val;
    val.type = Var_Type_Real;
    val.real = std::max(parameters[0].real, parameters[1].real);
    results[0] = val;
}

void get_default_builtin_functions(Function* list)
{
    static Variable_Type single_value[1] = { Var_Type_Real };
    static Variable_Type two_values[2] = { Var_Type_Real, Var_Type_Real };
    static Variable_Type three_values[3] = { Var_Type_Real, Var_Type_Real, Var_Type_Real };

    /*
    list[BUILTIN_FUNC_ABS] = Function(
        interp_fabs,
        FunctionSignature(String("abs"), make_array(single_value), make_array(single_value))
    );
    list[BUILTIN_FUNC_SIGN] = Function(
        interp_get_sign,
        FunctionSignature(String("sign"), make_array(single_value), make_array(single_value))
    );
    list[BUILTIN_FUNC_CEIL] = Function(
        interp_ceil,
        FunctionSignature(String("ceil"), make_array(single_value), make_array(single_value))
    );
    list[BUILTIN_FUNC_FLOOR] = Function(
        interp_floor,
        FunctionSignature(String("floor"), make_array(single_value), make_array(single_value))
    );
    list[BUILTIN_FUNC_SIN] = Function(
        interp_sin,
        FunctionSignature(String("sin"), make_array(single_value), make_array(single_value))
    );
    list[BUILTIN_FUNC_COS] = Function(
        interp_cos,
        FunctionSignature(String("cos"), make_array(single_value), make_array(single_value))
    );
    list[BUILTIN_FUNC_ARCSIN] = Function(
        interp_asin,
        FunctionSignature(String("asin"), make_array(single_value), make_array(single_value))
    );
    list[BUILTIN_FUNC_ARCCOS] = Function(
        interp_acos,
        FunctionSignature(String("acos"), make_array(single_value), make_array(single_value))
    );
    list[BUILTIN_FUNC_EXP] = Function(
        interp_exp,
        FunctionSignature(String("exp"), make_array(single_value), make_array(single_value))
    );
    list[BUILTIN_FUNC_SMOOTHSTEP] = Function(
        interp_smoothstep,
        FunctionSignature(String("smoothstep"), make_array(single_value), make_array(single_value))
    );
    list[BUILTIN_FUNC_CLAMP] = Function(
        interp_clamp,
        FunctionSignature(String("clamp"), make_array(single_value), make_array(three_values))
    );
    list[BUILTIN_FUNC_POW] = Function(
    	interp_pow,
    	FunctionSignature(String("pow"), make_array(single_value), make_array(two_values))
    );
    list[BUILTIN_FUNC_FRACT] = Function(
    	interp_fract,
    	FunctionSignature(String("fract"), make_array(single_value), make_array(single_value))
    );
    list[BUILTIN_FUNC_MIX] = Function(
        interp_mix,
        FunctionSignature(String("mix"), make_array(single_value), make_array(three_values))
    );
    list[BUILTIN_FUNC_SAW] = Function(
        interp_saw,
        FunctionSignature(String("saw"), make_array(single_value), make_array(single_value))
    );
    list[BUILTIN_FUNC_SQUARE] = Function(
        interp_square,
        FunctionSignature(String("square"), make_array(single_value), make_array(single_value))
    );
    list[BUILTIN_FUNC_TRIANGLE] = Function(
        interp_triangle,
        FunctionSignature(String("triangle"), make_array(single_value), make_array(single_value))
    );
    list[BUILTIN_FUNC_TAN] = Function(
        interp_tan,
        FunctionSignature(String("tan"), make_array(single_value), make_array(single_value))
    );
    list[BUILTIN_FUNC_ARCTAN] = Function(
        interp_atan,
        FunctionSignature(String("atan"), make_array(single_value), make_array(single_value))
    );
    list[BUILTIN_FUNC_LOG] = Function(
        interp_log,
        FunctionSignature(String("log"), make_array(single_value), make_array(single_value))
    );
    list[BUILTIN_FUNC_MAX] = Function(
        interp_max,
        FunctionSignature(String("max"), make_array(single_value), make_array(two_values))
    );
    list[BUILTIN_FUNC_MIN] = Function(
        interp_min,
        FunctionSignature(String("min"), make_array(single_value), make_array(two_values))
    );
    */
}

bool is_builtin_function(const Expr_Call* call) {
    return call->fn_id < BUILTIN_FUNC_COUNT;
}

bool is_builtin_variable(const Expr_Variable* var)
{
    return var->var_id < BUILTIN_VARIABLE_COUNT;
}

const char* get_builtin_function_name(Builtin_Func_Type builtin)
{
    switch (builtin)
    {
        case BUILTIN_FUNC_EXP:         { return "exponential"; }
        case BUILTIN_FUNC_ABS:         { return "absolute value"; }
        case BUILTIN_FUNC_SIGN:        { return "sign"; }
        case BUILTIN_FUNC_CEIL:        { return "ceil"; }
        case BUILTIN_FUNC_FLOOR:       { return "floor"; }
        case BUILTIN_FUNC_SIN:         { return "sin"; }
        case BUILTIN_FUNC_COS:         { return "cos"; }
        case BUILTIN_FUNC_TAN:         { return "tan"; }
        case BUILTIN_FUNC_ARCSIN:      { return "arcsin"; }
        case BUILTIN_FUNC_ARCCOS:      { return "arccos"; }
        case BUILTIN_FUNC_ARCTAN:      { return "arctan"; }
        case BUILTIN_FUNC_FRACT:       { return "fract"; }
        case BUILTIN_FUNC_SMOOTHSTEP:  { return "smoothstep"; }
        case BUILTIN_FUNC_MIX:         { return "mix"; }
        case BUILTIN_FUNC_SAW:         { return "saw"; }
        case BUILTIN_FUNC_SQUARE:      { return "square"; }
        case BUILTIN_FUNC_TRIANGLE:    { return "triangle"; }
        case BUILTIN_FUNC_CLAMP:       { return "clamp"; }
        case BUILTIN_FUNC_POW:         { return "pow"; }
        case BUILTIN_FUNC_LOG:         { return "natural log"; }
        case BUILTIN_FUNC_MIN:         { return "min"; }
        case BUILTIN_FUNC_MAX:         { return "max"; }
        case BUILTIN_FUNC_UNKNOWN:     { return "unknown function"; }
        default:
            return "Builtin_Function_Not_Registered";
    }
}
