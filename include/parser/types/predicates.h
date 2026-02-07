#ifndef PARSER_TYPES_PREDICATES_H
#define PARSER_TYPES_PREDICATES_H

#include <parser/types/core.h>
#include <stdbool.h>

bool type_equals(csq_type* a, csq_type* b);
bool type_is_integer(csq_type* type);
bool type_is_unsigned(csq_type* type);
bool type_is_float(csq_type* type);
bool type_is_numeric(csq_type* type);
bool type_is_primitive(csq_type* type);
bool type_is_array(csq_type* type);

#endif
