#include "csquare/lexer/lexer.h"
#include "csquare/tests/tests.h"
#include <stdlib.h>
#include <string.h>

TEST(test_percent_error) {
  const char *src = "void main() {\n    %\n}";
  token_list *tokens = lex("memsrc", src);

  token *percent = NULL;

  for (size_t i = 0; i < tokens->count; i++) {
    if (tokens->tokens[i]->start[0] == '%') {
      percent = tokens->tokens[i];
      break;
    }
  }

  ASSERT(percent != NULL, "Percent token not found");
  ASSERT(percent->type == T_ERROR, "Percent token type not ERROR");

  free_token_list(tokens);
  return 0;
}
