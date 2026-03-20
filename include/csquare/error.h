#ifndef ERROR_H
#define ERROR_H

#include <stddef.h>

typedef enum {
  ERROR_NONE,
  SYNERR_UNKNOWN_CHARACTER,
  SYNERR_UNTERMINATED_STRING,
  SYNERR_UNEXPECTED_TOKEN,
  SYNERR_NOT_A_LITERAL,
  SEMERR_LITERAL_NOT_AN_UINT,
  INERR_CURR_NODE_NULL,
  INERR_PARSED_NULL,
  INERR_PEEK_OOB,
  INERR_ARENA_ALLOC_NULL,
  INERR_NULL_PTR
} error_type;

typedef enum { ERROR_LEVEL_WARNING, ERROR_LEVEL_ERROR } error_level;

typedef struct {
  int line;
  int col;
} file_pos;

typedef struct {
  const char *ptr;
  size_t len;
} text_span;

typedef struct {
  const char *message;
  error_type type;
  error_level level;

  const char *file;
  file_pos pos;

  text_span line;
  text_span highlight;
} error_info;

error_info new_error_info(const char *msg, error_type type, error_level level,
                          const char *file, int line, int col,
                          const char *line_str, int highlight_start,
                          int highlight_len);

void print_error(const error_info *err);

// Instead of using an error_info object, simply throw the error directly.
//
// Less parameters and hassle, no object. Used for miscellaneous errors or
// errors that don't need more than a message and a location, normally
// internal errors.
void simple_error(const char *msg, int line, const char *file, error_type type,
                  error_level level);

// Same as simple_error, but automatically exits with code 1.
void simple_fatal(const char *msg, int line, const char *file, error_type type,
                  error_level level);

#endif
