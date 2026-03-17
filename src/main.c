#include "csquare/error.h"
#include "csquare/lexer/lexer.h"
#include "csquare/opt-common.h"
#include "csquare/parser/parser.h"
#include <stdio.h>
#include <stdlib.h>

char *read_file(const char *filename) {
  FILE *f = fopen(filename, "rb");
  if (!f) {
    perror("fopen");
    return NULL;
  }
  fseek(f, 0, SEEK_END);
  long size = ftell(f);
  fseek(f, 0, SEEK_SET);

  char *buffer = malloc(size + 1);
  if (!buffer) {
    perror("malloc");
    fclose(f);
    return NULL;
  }

  if (fread(buffer, 1, size, f) != (size_t)size) {
    perror("fread");
    free(buffer);
    fclose(f);
    return NULL;
  }
  buffer[size] = '\0';
  fclose(f);
  return buffer;
}

const char *get_line(const char *src, int target_line, size_t *out_len) {
  if (target_line < 1)
    return NULL;

  int line = 1;
  const char *p = src;

  while (*p) {
    if (line == target_line) {
      const char *line_start = p;
      size_t len = 0;
      while (p[len] && p[len] != '\n')
        len++;
      if (out_len)
        *out_len = len;
      return line_start;
    }

    if (*p == '\n')
      line++;
    p++;
  }

  if (out_len)
    *out_len = 0;
  return NULL;
}

int main(int argc, char *argv[]) {
  csq_options *opts = options_parse(argc, argv);
  if (!opts)
    return EXIT_FAILURE;

  if (argc < 2) {
    fprintf(stderr, "Usage: %s <source-file>\n", argv[0]);
    return EXIT_FAILURE;
  }

  const char *filename = argv[1];
  char *src = read_file(filename);
  if (!src)
    return EXIT_FAILURE;

  token_list *lexed = lex(src);

  for (size_t i = 0; i < lexed->count; i++) {
    token *tk = lexed->tokens[i];
    if (tk->type == T_ERROR) {
      printf("\n");
      size_t line_len;
      const char *line = get_line(src, tk->line, &line_len);
      if (!line) {
        line = "";
        line_len = 0;
      }

      int highlight_start = tk->start - line;
      int highlight_len = tk->length;

      error_info e = new_error_info(tk->errmsg, tk->errtype, ERROR_LEVEL_ERROR,
                                    filename, tk->line, tk->col, line,
                                    highlight_start, highlight_len);
      print_error(&e);
    }
  }

  parser *p = new_parser(lexed, filename, src);

  free_token_list(lexed);
  free_parser(p);
  free(src);
  return 0;
}
