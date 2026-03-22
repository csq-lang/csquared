#include "csquare/error.h"
#include "csquare/lexer/lexer.h"

token *lex_string(const char *filename, const char *p, int *len, int *line,
                  int *col) {
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
    *col = (*col) + (*len);

    const char *prefix = "Unterminated string: ";
    char *msg = malloc(strlen(prefix) + *len + 1);

    sprintf(msg, "Unterminated string: %.*s", *len, start);
    return error_token(filename, *line, *col, E_UNTERMINATED_STRING);
  }

  p++;

  *len = (int)(p - start);
  *col = (*col) + (*len);
  return new_token(start, *len, T_STRING);
}
