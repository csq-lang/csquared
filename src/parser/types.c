#include <parser/types.h>
#include <stdlib.h>
#include <string.h>

static rvn_type *type_primitive(type_kind kind, size_t size, size_t align) {
  rvn_type *type = calloc(1, sizeof(rvn_type));
  if (!type)
    return NULL;

  type->kind = kind;
  type->size = size;
  type->align = align;
  type->is_const = false;

  return type;
}

rvn_type *type_create(type_kind kind) { return type_primitive(kind, 0, 0); }

void type_free(rvn_type *type) {
  if (!type)
    return;

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

rvn_type *type_void(void) { return type_primitive(TYPE_VOID, 0, 0); }

rvn_type *type_bool(void) { return type_primitive(TYPE_BOOL, 1, 1); }

rvn_type *type_i8(void) { return type_primitive(TYPE_I8, 1, 1); }

rvn_type *type_i16(void) { return type_primitive(TYPE_I16, 2, 2); }

rvn_type *type_i32(void) { return type_primitive(TYPE_I32, 4, 4); }

rvn_type *type_i64(void) { return type_primitive(TYPE_I64, 8, 8); }

rvn_type *type_u8(void) { return type_primitive(TYPE_U8, 1, 1); }

rvn_type *type_u16(void) { return type_primitive(TYPE_U16, 2, 2); }

rvn_type *type_u32(void) { return type_primitive(TYPE_U32, 4, 4); }

rvn_type *type_u64(void) { return type_primitive(TYPE_U64, 8, 8); }

rvn_type *type_f32(void) { return type_primitive(TYPE_F32, 4, 4); }

rvn_type *type_f64(void) { return type_primitive(TYPE_F64, 8, 8); }

rvn_type *type_char(void) { return type_primitive(TYPE_CHAR, 1, 1); }

rvn_type *type_string(void) {
  return type_primitive(TYPE_STRING, sizeof(void *) * 2, sizeof(void *));
}

rvn_type *type_tag(void) {
  return type_primitive(TYPE_TAG, sizeof(void *), sizeof(void *));
}

rvn_type *type_pointer(rvn_type *base) {
  rvn_type *type = type_primitive(TYPE_POINTER, sizeof(void *), sizeof(void *));
  if (!type)
    return NULL;

  type->data.pointer.base = base;
  return type;
}

rvn_type *type_array(rvn_type *elem, size_t count) {
  rvn_type *type = type_primitive(TYPE_ARRAY, 0, 0);
  if (!type)
    return NULL;

  type->data.array.elem = elem;
  type->data.array.count = count;
  type->size = elem->size * count;
  type->align = elem->align;

  return type;
}

rvn_type *type_slice(rvn_type *elem) {
  rvn_type *type =
      type_primitive(TYPE_SLICE, sizeof(void *) * 2, sizeof(void *));
  if (!type)
    return NULL;

  type->data.slice.elem = elem;
  return type;
}

rvn_type *type_map(rvn_type *key, rvn_type *value) {
  rvn_type *type = type_primitive(TYPE_MAP, sizeof(void *), sizeof(void *));
  if (!type)
    return NULL;

  type->data.map.key = key;
  type->data.map.value = value;
  return type;
}

rvn_type *type_function(type_list *params, rvn_type *ret, bool is_variadic) {
  rvn_type *type =
      type_primitive(TYPE_FUNCTION, sizeof(void *), sizeof(void *));
  if (!type)
    return NULL;

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

rvn_type *type_named(const char *name, size_t len) {
  rvn_type *type = type_primitive(TYPE_NAMED, 0, 0);
  if (!type)
    return NULL;

  type->data.named.name = name;
  type->data.named.name_len = len;
  type->data.named.resolved = NULL;

  return type;
}

bool type_equals(rvn_type *a, rvn_type *b) {
  if (a == b)
    return true;
  if (!a || !b)
    return false;
  if (a->kind != b->kind)
    return false;

  switch (a->kind) {
  case TYPE_POINTER:
    return type_equals(a->data.pointer.base, b->data.pointer.base);
  case TYPE_ARRAY:
    return a->data.array.count == b->data.array.count &&
           type_equals(a->data.array.elem, b->data.array.elem);
  case TYPE_SLICE:
    return type_equals(a->data.slice.elem, b->data.slice.elem);
  case TYPE_MAP:
    return type_equals(a->data.map.key, b->data.map.key) &&
           type_equals(a->data.map.value, b->data.map.value);
  case TYPE_FUNCTION:
    if (a->data.function.params.count != b->data.function.params.count)
      return false;
    for (size_t i = 0; i < a->data.function.params.count; i++) {
      if (!type_equals(a->data.function.params.items[i],
                       b->data.function.params.items[i]))
        return false;
    }
    return type_equals(a->data.function.ret, b->data.function.ret);
  case TYPE_NAMED:
    if (a->data.named.name_len != b->data.named.name_len)
      return false;
    return memcmp(a->data.named.name, b->data.named.name,
                  a->data.named.name_len) == 0;
  default:
    return true;
  }
}

bool type_is_integer(rvn_type *type) {
  if (!type)
    return false;
  return type->kind >= TYPE_I8 && type->kind <= TYPE_U64;
}

bool type_is_unsigned(rvn_type *type) {
  if (!type)
    return false;
  return type->kind >= TYPE_U8 && type->kind <= TYPE_U64;
}

bool type_is_float(rvn_type *type) {
  if (!type)
    return false;
  return type->kind == TYPE_F32 || type->kind == TYPE_F64;
}

bool type_is_numeric(rvn_type *type) {
  return type_is_integer(type) || type_is_float(type);
}

bool type_is_primitive(rvn_type *type) {
  if (!type)
    return false;
  return type->kind <= TYPE_TAG;
}

size_t type_sizeof(rvn_type *type) {
  if (!type)
    return 0;
  return type->size;
}

size_t type_alignof(rvn_type *type) {
  if (!type)
    return 0;
  return type->align;
}

void type_list_init(type_list *list) {
  list->items = NULL;
  list->count = 0;
  list->capacity = 0;
}

void type_list_free(type_list *list) {
  if (!list)
    return;

  for (size_t i = 0; i < list->count; i++) {
    type_free(list->items[i]);
  }
  free(list->items);
  list->items = NULL;
  list->count = 0;
  list->capacity = 0;
}

void type_list_add(type_list *list, rvn_type *type) {
  if (!list || !type)
    return;

  if (list->count >= list->capacity) {
    size_t new_capacity = list->capacity == 0 ? 8 : list->capacity * 2;
    rvn_type **new_items =
        realloc(list->items, new_capacity * sizeof(rvn_type *));
    if (!new_items)
      return;

    list->items = new_items;
    list->capacity = new_capacity;
  }

  list->items[list->count++] = type;
}

void type_field_list_init(type_field_list *list) {
  list->items = NULL;
  list->count = 0;
  list->capacity = 0;
}

void type_field_list_free(type_field_list *list) {
  if (!list)
    return;

  for (size_t i = 0; i < list->count; i++) {
    free((void *)list->items[i].name);
  }
  free(list->items);
  list->items = NULL;
  list->count = 0;
  list->capacity = 0;
}

void type_field_list_add(type_field_list *list, const char *name, size_t len,
                         rvn_type *type) {
  if (!list)
    return;

  if (list->count >= list->capacity) {
    size_t new_capacity = list->capacity == 0 ? 8 : list->capacity * 2;
    type_field *new_items =
        realloc(list->items, new_capacity * sizeof(type_field));
    if (!new_items)
      return;

    list->items = new_items;
    list->capacity = new_capacity;
  }

  char *name_copy = malloc(len + 1);
  if (!name_copy)
    return;
  memcpy(name_copy, name, len);
  name_copy[len] = '\0';

  list->items[list->count].name = name_copy;
  list->items[list->count].name_len = len;
  list->items[list->count].type = type;
  list->count++;
}
