/**
 * @file primitives.c
 * @brief Primitive type constructors
 * @details Provides factory functions for creating all primitive types
 * including integers, floats, booleans, strings, and tags.
 */

#include <parser/types/primitives.h>
#include <parser/types/internal.h>

csq_type* type_void(void) {
    return type_primitive(TYPE_VOID, 0, 0);
}

csq_type* type_bool(void) {
    return type_primitive(TYPE_BOOL, 1, 1);
}

csq_type* type_int(void) {
    return type_primitive(TYPE_INT, sizeof(long long), sizeof(long long));
}

csq_type* type_f32(void) {
    return type_primitive(TYPE_F32, 4, 4);
}

csq_type* type_f64(void) {
    return type_primitive(TYPE_F64, 8, 8);
}

csq_type* type_string(void) {
    return type_primitive(TYPE_STRING, sizeof(void*) * 2, sizeof(void*));
}

csq_type* type_tag(void) {
    return type_primitive(TYPE_TAG, sizeof(void*), sizeof(void*));
}
