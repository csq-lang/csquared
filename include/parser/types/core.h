#ifndef PARSER_TYPES_CORE_H
#define PARSER_TYPES_CORE_H

#include <stddef.h>
#include <stdbool.h>

typedef struct csq_type csq_type;

typedef enum {
    TYPE_VOID,
    TYPE_BOOL,
    TYPE_INT,
    TYPE_F32,
    TYPE_F64,
    TYPE_STRING,
    TYPE_TAG,
    TYPE_POINTER,
    TYPE_ARRAY,
    TYPE_SLICE,
    TYPE_MAP,
    TYPE_FUNCTION,
    TYPE_STRUCT,
    TYPE_ENUM,
    TYPE_UNION,
    TYPE_TUPLE,
    TYPE_GENERIC,
    TYPE_GENERIC_PARAM,
    TYPE_NAMED,
    TYPE_INFERRED,
    TYPE_ERROR
} type_kind;

typedef struct {
    csq_type** items;
    size_t count;
    size_t capacity;
} type_list;

typedef struct {
    const char* name;
    size_t name_len;
    csq_type* type;
} type_field;

typedef struct {
    type_field* items;
    size_t count;
    size_t capacity;
} type_field_list;

struct csq_type {
    type_kind kind;
    size_t size;
    size_t align;
    bool is_const;
    
    union {
        struct {
            csq_type* base;
        } pointer;
        
        struct {
            csq_type* elem;
            size_t count;
        } array;
        
        struct {
            csq_type* elem;
        } slice;
        
        struct {
            csq_type* key;
            csq_type* value;
        } map;
        
        struct {
            type_list params;
            csq_type* ret;
            bool is_variadic;
            bool is_async;
        } function;
        
        struct {
            const char* name;
            size_t name_len;
            type_field_list fields;
            csq_type* parent_scope;
        } struct_type;
        
        struct {
            const char* name;
            size_t name_len;
            type_field_list variants;
        } enum_type;
        
        struct {
            type_list members;
        } tuple;
        
        struct {
            const char* name;
            size_t name_len;
            csq_type* constraint;
        } generic_param;
        
        struct {
            csq_type* base;
            type_list args;
        } generic_inst;
        
        struct {
            const char* name;
            size_t name_len;
            csq_type* resolved;
        } named;
    } data;
};

csq_type* type_create(type_kind kind);
void type_free(csq_type* type);

#endif
