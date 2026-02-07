/**
 * @file composite.c
 * @brief Composite type constructors
 * @details Implements creation of composite types including pointers, arrays,
 * slices, maps, function types, and named types.
 */

#include <parser/types/composite.h>
#include <parser/types/internal.h>
#include <parser/types/primitives.h>
#include <parser/types/lists.h>
#include <stdlib.h>

csq_type* type_pointer(csq_type* base) {
    csq_type* type = type_primitive(TYPE_POINTER, sizeof(void*), sizeof(void*));
    if (!type) return NULL;
    
    type->data.pointer.base = base;
    return type;
}

csq_type* type_array(csq_type* elem, size_t count) {
    csq_type* type = type_primitive(TYPE_ARRAY, 0, 0);
    if (!type) return NULL;
    
    type->data.array.elem = elem;
    type->data.array.count = count;
    type->size = elem->size * count;
    type->align = elem->align;
    
    return type;
}

csq_type* type_slice(csq_type* elem) {
    csq_type* type = type_primitive(TYPE_SLICE, sizeof(void*) * 2, sizeof(void*));
    if (!type) return NULL;
    
    type->data.slice.elem = elem;
    return type;
}

csq_type* type_map(csq_type* key, csq_type* value) {
    csq_type* type = type_primitive(TYPE_MAP, sizeof(void*), sizeof(void*));
    if (!type) return NULL;
    
    type->data.map.key = key;
    type->data.map.value = value;
    return type;
}

csq_type* type_function(type_list* params, csq_type* ret, bool is_variadic) {
    csq_type* type = type_primitive(TYPE_FUNCTION, sizeof(void*), sizeof(void*));
    if (!type) return NULL;
    
    type_list_init(&type->data.function.params);
    if (params) {
        for (size_t i = 0; i < params->count; i++) {
            type_list_add(&type->data.function.params, params->items[i]);
        }
    }
    type->data.function.ret = ret ? ret : type_void();
    type->data.function.is_variadic = is_variadic;
    type->data.function.is_async = false;
    
    return type;
}

csq_type* type_named(const char* name, size_t len) {
    csq_type* type = type_primitive(TYPE_NAMED, 0, 0);
    if (!type) return NULL;
    
    type->data.named.name = name;
    type->data.named.name_len = len;
    type->data.named.resolved = NULL;
    
    return type;
}
