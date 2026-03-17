#include "csquare/error.h"
#include <stdio.h>
#include <string.h>

#include "csquare/error.h"
#include <string.h>

error_info new_error_info(const char *msg, error_type type, error_level level,
                          const char *file, int line, int col,
                          const char *line_str, int highlight_start,
                          int highlight_len) {
  error_info e;
  e.message = strdup(msg);
  e.type = type;
  e.level = level;
  e.file = file;
  e.pos = (file_pos){line, col};

  e.line.ptr = line_str;

  size_t line_len = 0;
  while (line_str[line_len] && line_str[line_len] != '\n')
    line_len++;
  e.line.len = line_len;

  e.highlight.ptr = line_str + highlight_start;
  e.highlight.len = highlight_len;

  return e;
}

#define CRED "\x1b[31m"
#define CYELLOW "\x1b[33m"
#define CCYAN "\x1b[36m"
#define CBLUE "\x1b[34m"
#define CRESET "\x1b[0m"

// static void print_span(const char *ptr, size_t len) {
//   fwrite(ptr, 1, len, stdout);
// }

void print_error(const error_info *e) {
  const char *level_color = (e->level == ERROR_LEVEL_ERROR) ? CRED : CYELLOW;

  printf("%s%s:%s %s\n", level_color,
         (e->level == ERROR_LEVEL_ERROR ? "error" : "warning"), CRESET,
         e->message);
  printf("  --> %s:%d:%d\n", e->file, e->pos.line, e->pos.col);
  printf("   |\n");

  printf("%4d | ", e->pos.line);

  fwrite(e->line.ptr, 1, e->highlight.ptr - e->line.ptr, stdout);

  printf(CRED);
  fwrite(e->highlight.ptr, 1, e->highlight.len, stdout);
  printf(CRESET);

  size_t rest_len =
      e->line.len - (e->highlight.ptr - e->line.ptr) - e->highlight.len;
  fwrite(e->highlight.ptr + e->highlight.len, 1, rest_len, stdout);
  printf("\n");

  printf("     | ");
  for (size_t i = 0; i < e->highlight.ptr - e->line.ptr; i++)
    putchar((e->line.ptr[i] == '\t') ? '\t' : ' ');
  printf(CRED);
  for (size_t i = 0; i < e->highlight.len; i++)
    putchar('^');
  printf(CRESET "\n");
}
