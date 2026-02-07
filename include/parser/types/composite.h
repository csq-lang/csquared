#ifndef PARSER_TYPES_COMPOSITE_H
#define PARSER_TYPES_COMPOSITE_H

#include <parser/types/core.h>
#include <stdbool.h>

csq_type* type_pointer(csq_type* base);
csq_type* type_array(csq_type* elem, size_t count);
csq_type* type_slice(csq_type* elem);
csq_type* type_map(csq_type* key, csq_type* value);
csq_type* type_function(type_list* params, csq_type* ret, bool is_variadic);
csq_type* type_named(const char* name, size_t len);

#endif
