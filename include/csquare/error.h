#ifndef ERROR_H
#define ERROR_H

#include <stddef.h>

typedef enum {
  ERROR_NONE,
  SYNERR_UNKNOWN_CHARACTER,
  SYNERR_UNTERMINATED_STRING
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

#endif
