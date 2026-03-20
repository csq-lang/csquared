#include "csquare/error.h"
#include "csquare/lexer/lexer.h"
#include "csquare/opt-common.h"
#include "csquare/parser/parser.h"
#include "csquare/utils.h"
#include <stdio.h>
#include <stdlib.h>

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
    // print_token(tk);
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
  // parse(p);

  free_token_list(lexed);
  free_parser(p);
  free(src);
  return 0;
}
