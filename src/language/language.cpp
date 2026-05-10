#include "lang.hpp"
#include "parse.hpp"
#include "token.hpp"
#include "statement.hpp"

bool Value::evaluate_truth_value() {
    switch (type) {
        case Var_Type_Boolean: return boolean;
        case Var_Type_Integer: return integer != 0;
        case Var_Type_Real: return real != 0.0;
    }

    panic("Unknown value type");
}
