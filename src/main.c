#include "csquare/error.h"
#include "csquare/lexer/lexer.h"
#include "csquare/opt-common.h"
#include "csquare/parser/parser.h"
#include "csquare/tests/tests.h"
#include "csquare/utils.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
  csq_options *opts = options_parse(argc, argv);
  if (!opts)
    return EXIT_FAILURE;

#ifdef CSQ_RUN_TESTS
  return tests_main();
#else
  if (argc < 2) {
    fprintf(stderr, "Usage: %s <source-file>\n", argv[0]);
    return EXIT_FAILURE;
  }

  const char *filename = argv[1];
  char *src = read_file(filename);
  if (!src)
    return EXIT_FAILURE;

  token_list *lexed = lex(filename, src);

  for (size_t i = 0; i < lexed->count; i++) {
    token *tk = lexed->tokens[i];
    // print_token(tk);
    if (tk->type == T_ERROR) {
      print_error(tk->e);
    }
  }

  parser *p = new_parser(lexed, filename, src);
  // parse(p);

  free_token_list(lexed);
  free_parser(p);
  free(src);
  return 0;
#endif
}
