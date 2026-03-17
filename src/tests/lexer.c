#include "csquare/lexer/lexer.h"
#include "csquare/tests/tests.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int test_percent_error(void) {
  const char *src = "void main() {\n    %\n}";
  token_list *tokens = lex(src);

  token *percent = NULL;
  for (size_t i = 0; i < tokens->count; i++) {
    if (tokens->tokens[i]->start[0] == '%') {
      percent = tokens->tokens[i];
      break;
    }
  }

  ASSERT(percent != NULL, "Percent token not found", NULL);
  ASSERT(percent->type == T_ERROR, "Percent token type not ERROR", NULL);
  ASSERT(percent->line == 2, "Percent token line mismatch (got %d)",
         percent->line);
  ASSERT(percent->col == 5,
         "Percent token column mismatch (got %d, expected %d)", percent->col,
         5);
  ASSERT(strcmp(percent->errmsg, "unknown character \x1b[32m'%'\x1b[0m") == 0,
         "Percent token errmsg mismatch", NULL);

  free_token_list(tokens);
  TEST_PASS;
  return 0;
}

int test_multiple_errors(void) {
  const char *src = "abc $ % @";
  token_list *tokens = lex(src);

  const char *expected_msgs[] = {"unknown character \x1b[32m'$'\x1b[0m",
                                 "unknown character \x1b[32m'%'\x1b[0m",
                                 "unknown character \x1b[32m'@'\x1b[0m"};
  int found = 0;

  for (size_t i = 0; i < tokens->count; i++) {
    token *tk = tokens->tokens[i];
    if (tk->type == T_ERROR) {
      ASSERT(found < 3, "Too many error tokens found", NULL);
      ASSERT(strcmp(tk->errmsg, expected_msgs[found]) == 0,
             "Error token message mismatch (got \"%s\", expected \"%s\")",
             tk->errmsg, expected_msgs[found]);
      found++;
    }
  }

  ASSERT(found == 3, "Did not find all expected error tokens", NULL);

  free_token_list(tokens);
  TEST_PASS;
  return 0;
}
