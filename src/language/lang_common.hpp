#ifndef _LANG_COMMON
#define _LANG_COMMON

#include "common.hpp"

struct Variable {
    String name = {};
    String type_name = {};

    Variable() {}
    Variable(String n, String p_type) : name(n), type_name(p_type) {}

    bool operator==(const Variable& other) const {
        return type_name == other.type_name && name == other.name;
    }
};

#endif // _LANG_COMMON
