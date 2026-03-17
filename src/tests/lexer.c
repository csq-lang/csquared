#include "csquare/lexer/lexer.h"
#include "csquare/tests/tests.h"
#include <stdlib.h>
#include <string.h>

TEST(test_percent_error) {
  const char *src = "void main() {\n    %\n}";
  token_list *tokens = lex(src);

  token *percent = NULL;

  for (size_t i = 0; i < tokens->count; i++) {
    if (tokens->tokens[i]->start[0] == '%') {
      percent = tokens->tokens[i];
      break;
    }
  }

  ASSERT(percent != NULL, "Percent token not found");
  ASSERT(percent->type == T_ERROR, "Percent token type not ERROR");
  ASSERT(percent->line == 2, "Line mismatch (got %d, expected 2)",
         percent->line);
  ASSERT(percent->col == 5, "Column mismatch (got %d, expected 5)",
         percent->col);

  ASSERT(strcmp(percent->errmsg, "unknown character \x1b[32m'%'\x1b[0m") == 0,
         "Error message mismatch (got \"%s\")", percent->errmsg);

  free_token_list(tokens);
  return 0;
}

TEST(test_multiple_errors) {
  const char *src = "abc $ % @";
  token_list *tokens = lex(src);

  const char *expected_msgs[] = {"unknown character \x1b[32m'$'\x1b[0m",
                                 "unknown character \x1b[32m'%'\x1b[0m",
                                 "unknown character \x1b[32m'@'\x1b[0m"};

  int found = 0;

  for (size_t i = 0; i < tokens->count; i++) {
    token *tk = tokens->tokens[i];

    if (tk->type == T_ERROR) {
      ASSERT(found < 3, "Too many error tokens found");

      ASSERT(strcmp(tk->errmsg, expected_msgs[found]) == 0,
             "Message mismatch\n"
             "        got:      \"%s\"\n"
             "        expected: \"%s\"",
             tk->errmsg, expected_msgs[found]);

      found++;
    }
  }

  ASSERT(found == 3, "Expected 3 error tokens, got %d", found);

  free_token_list(tokens);
  return 0;
}
