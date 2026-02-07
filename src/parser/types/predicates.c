/**
 * @file predicates.c
 * @brief Type predicate functions
 * @details Implements type classification and comparison functions for
 * semantic analysis and type checking.
 */

#include <parser/types/predicates.h>
#include <parser/types/core.h>
#include <string.h>

/**
 * @brief Check if two types are equivalent
 * @param a First type
 * @param b Second type
 * @return True if types are equivalent
 */
bool type_equals(csq_type* a, csq_type* b) {
    if (a == b) return true;
    if (!a || !b) return false;
    if (a->kind != b->kind) return false;
    
    switch (a->kind) {
        case TYPE_POINTER:
            return type_equals(a->data.pointer.base, b->data.pointer.base);
        case TYPE_ARRAY:
            return a->data.array.count == b->data.array.count &&
                   type_equals(a->data.array.elem, b->data.array.elem);
        case TYPE_SLICE:
            return type_equals(a->data.slice.elem, b->data.slice.elem);
        case TYPE_MAP:
            return type_equals(a->data.map.key, b->data.map.key) &&
                   type_equals(a->data.map.value, b->data.map.value);
        case TYPE_FUNCTION:
            if (a->data.function.params.count != b->data.function.params.count) return false;
            for (size_t i = 0; i < a->data.function.params.count; i++) {
                if (!type_equals(a->data.function.params.items[i], b->data.function.params.items[i]))
                    return false;
            }
            return type_equals(a->data.function.ret, b->data.function.ret);
        case TYPE_NAMED:
            if (a->data.named.name_len != b->data.named.name_len) return false;
            return memcmp(a->data.named.name, b->data.named.name, a->data.named.name_len) == 0;
        default:
            return true;
    }
}

/**
 * @brief Check if type is an integer type
 * @param type Type to check
 * @return True if type is signed or unsigned integer
 */
bool type_is_integer(csq_type* type) {
    if (!type) return false;
    return type->kind == TYPE_INT;
}

bool type_is_unsigned(csq_type* type) {
    if (!type) return false;
    return false; // No unsigned integer type anymore
}

/**
 * @brief Check if type is a floating-point type
 * @param type Type to check
 * @return True if type is f32 or f64
 */
bool type_is_float(csq_type* type) {
    if (!type) return false;
    return type->kind == TYPE_F32 || type->kind == TYPE_F64;
}

bool type_is_numeric(csq_type* type) {
    return type_is_integer(type) || type_is_float(type);
}

bool type_is_primitive(csq_type* type) {
    if (!type) return false;
    return type->kind <= TYPE_TAG;
}

bool type_is_array(csq_type* type) {
    if (!type) return false;
    return type->kind == TYPE_ARRAY;
}
