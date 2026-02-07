/**
 * @file core.c
 * @brief Core type system functionality
 * @details Implements basic type creation, destruction, and type structure management.
 */

#include <parser/types/core.h>
#include <parser/types/lists.h>
#include <stdlib.h>

/**
 * @brief Create a primitive type with size and alignment
 * @param kind Type kind enumeration
 * @param size Size in bytes
 * @param align Alignment requirement in bytes
 * @return Allocated type structure
 */
csq_type* type_primitive(type_kind kind, size_t size, size_t align) {
    csq_type* type = calloc(1, sizeof(csq_type));
    if (!type) return NULL;
    
    type->kind = kind;
    type->size = size;
    type->align = align;
    type->is_const = false;
    
    return type;
}

/**
 * @brief Create a type with zero size and alignment
 * @param kind Type kind enumeration
 * @return Allocated type structure
 */
csq_type* type_create(type_kind kind) {
    return type_primitive(kind, 0, 0);
}

/**
 * @brief Free type and its sub-types
 * @details Recursively deallocates type and dependent types.
 * @param type Type to deallocate
 */
void type_free(csq_type* type) {
    if (!type) return;
    
    switch (type->kind) {
        case TYPE_POINTER:
            type_free(type->data.pointer.base);
            break;
        case TYPE_ARRAY:
            type_free(type->data.array.elem);
            break;
        case TYPE_SLICE:
            type_free(type->data.slice.elem);
            break;
        case TYPE_MAP:
            type_free(type->data.map.key);
            type_free(type->data.map.value);
            break;
        case TYPE_FUNCTION:
            type_list_free(&type->data.function.params);
            type_free(type->data.function.ret);
            break;
        case TYPE_STRUCT:
            type_field_list_free(&type->data.struct_type.fields);
            break;
        case TYPE_ENUM:
            type_field_list_free(&type->data.enum_type.variants);
            break;
        case TYPE_TUPLE:
            type_list_free(&type->data.tuple.members);
            break;
        case TYPE_GENERIC_PARAM:
            type_free(type->data.generic_param.constraint);
            break;
        case TYPE_NAMED:
            type_free(type->data.named.resolved);
            break;
        default:
            break;
    }
    
    free(type);
}
