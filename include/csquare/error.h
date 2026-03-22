#ifndef ERROR_H
#define ERROR_H

#include <stddef.h>

#define E(NAME, STR) NAME,
#define ERROR_NAMES                                                            \
  E(E_UNKNOWN, "unknown error")                                                \
  E(E_UNKNOWN_CHARACTER, "unknown character")                                  \
  E(E_UNTERMINATED_STRING, "unterminated string")                              \
  E(E_UNEXPECTED_TOKEN, "unexpected token")                                    \
  E(E_PEEK_OUT_OF_BOUNDS, "peek out of bounds")                                \
  E(E_NULLPTR, "null pointer")                                                 \
  E(E_ARENA_ALLOC_FAIL, "arena allocation failed")

typedef enum { ERROR_NAMES E__COUNT } error_type;

#undef E

extern const char *error_type_str[];

typedef struct {
  error_type type;
  const char *filename;
  int line;

  int col;
  bool has_col;

  char **notes;
  size_t note_count;
  size_t note_cap;

  enum { L_ERR, L_WARN } level;
} csq_error;

csq_error *new_error(error_type type, const char *filename, int line);
void set_col(csq_error *e, int col);
void add_note(csq_error *e, const char *note);
void print_error(csq_error *e);
void free_error(csq_error *e);

#define quick_error(TYPE, LEVEL)                                               \
  e = new_error(TYPE, __FILE__, __LINE__);                                     \
  e->level = LEVEL;

#endif
