#ifndef PARSER_TYPES_H
#define PARSER_TYPES_H

#include <stdbool.h>
#include <stddef.h>

typedef struct rvn_type rvn_type;

typedef enum {
  TYPE_VOID,
  TYPE_BOOL,
  TYPE_I8,
  TYPE_I16,
  TYPE_I32,
  TYPE_I64,
  TYPE_U8,
  TYPE_U16,
  TYPE_U32,
  TYPE_U64,
  TYPE_F32,
  TYPE_F64,
  TYPE_CHAR,
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
  rvn_type **items;
  size_t count;
  size_t capacity;
} type_list;

typedef struct {
  const char *name;
  size_t name_len;
  rvn_type *type;
} type_field;

typedef struct {
  type_field *items;
  size_t count;
  size_t capacity;
} type_field_list;

struct rvn_type {
  type_kind kind;
  size_t size;
  size_t align;
  bool is_const;

  union {
    struct {
      rvn_type *base;
    } pointer;

    struct {
      rvn_type *elem;
      size_t count;
    } array;

    struct {
      rvn_type *elem;
    } slice;

    struct {
      rvn_type *key;
      rvn_type *value;
    } map;

    struct {
      type_list params;
      rvn_type *ret;
      bool is_variadic;
      bool is_async;
    } function;

    struct {
      const char *name;
      size_t name_len;
      type_field_list fields;
      rvn_type *parent_scope;
    } struct_type;

    struct {
      const char *name;
      size_t name_len;
      type_field_list variants;
    } enum_type;

    struct {
      type_list members;
    } tuple;

    struct {
      const char *name;
      size_t name_len;
      rvn_type *constraint;
    } generic_param;

    struct {
      rvn_type *base;
      type_list args;
    } generic_inst;

    struct {
      const char *name;
      size_t name_len;
      rvn_type *resolved;
    } named;
  } data;
};

rvn_type *type_create(type_kind kind);
void type_free(rvn_type *type);

rvn_type *type_void(void);
rvn_type *type_bool(void);
rvn_type *type_i8(void);
rvn_type *type_i16(void);
rvn_type *type_i32(void);
rvn_type *type_i64(void);
rvn_type *type_u8(void);
rvn_type *type_u16(void);
rvn_type *type_u32(void);
rvn_type *type_u64(void);
rvn_type *type_f32(void);
rvn_type *type_f64(void);
rvn_type *type_char(void);
rvn_type *type_string(void);
rvn_type *type_tag(void);

rvn_type *type_pointer(rvn_type *base);
rvn_type *type_array(rvn_type *elem, size_t count);
rvn_type *type_slice(rvn_type *elem);
rvn_type *type_map(rvn_type *key, rvn_type *value);
rvn_type *type_function(type_list *params, rvn_type *ret, bool is_variadic);
rvn_type *type_named(const char *name, size_t len);

bool type_equals(rvn_type *a, rvn_type *b);
bool type_is_integer(rvn_type *type);
bool type_is_unsigned(rvn_type *type);
bool type_is_float(rvn_type *type);
bool type_is_numeric(rvn_type *type);
bool type_is_primitive(rvn_type *type);

size_t type_sizeof(rvn_type *type);
size_t type_alignof(rvn_type *type);

void type_list_init(type_list *list);
void type_list_free(type_list *list);
void type_list_add(type_list *list, rvn_type *type);

void type_field_list_init(type_field_list *list);
void type_field_list_free(type_field_list *list);
void type_field_list_add(type_field_list *list, const char *name, size_t len,
                         rvn_type *type);

#endif
