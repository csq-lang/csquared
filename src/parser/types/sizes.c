/**
 * @file sizes.c
 * @brief Type size and alignment queries
 * @details Provides functions to query the size and alignment of types.
 */

#include <parser/types/sizes.h>
#include <parser/types/core.h>

size_t type_sizeof(csq_type* type) {
    if (!type) return 0;
    return type->size;
}

size_t type_alignof(csq_type* type) {
    if (!type) return 0;
    return type->align;
}
