#include "csquare/lexer/lexer.h"

token *lex_string(const char *p, int *len) {
  const char *start = p;
  char delim = *p;
  p++;

  while (*p != delim && *p != '\0') {
    if (*p == '\\' && *(p + 1) != '\0')
      p++;
    p++;
  }

  if (*p != delim) {
    *len = (int)(p - start);

    const char *prefix = "Unterminated string: ";
    char *msg = malloc(strlen(prefix) + *len + 1);

    sprintf(msg, "Unterminated string: %.*s", *len, start);
    return error_token(msg);
  }

  p++;

  *len = (int)(p - start);
  return new_token(start, *len, T_STRING);
}
