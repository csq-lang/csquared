#ifndef PARSER_TYPES_LISTS_H
#define PARSER_TYPES_LISTS_H

#include <parser/types/core.h>

void type_list_init(type_list* list);
void type_list_free(type_list* list);
void type_list_add(type_list* list, csq_type* type);

void type_field_list_init(type_field_list* list);
void type_field_list_free(type_field_list* list);
void type_field_list_add(type_field_list* list, const char* name, size_t len, csq_type* type);

#endif
