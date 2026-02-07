#ifndef PARSER_TYPES_SIZES_H
#define PARSER_TYPES_SIZES_H

#include <parser/types/core.h>
#include <stddef.h>

size_t type_sizeof(csq_type* type);
size_t type_alignof(csq_type* type);

#endif
