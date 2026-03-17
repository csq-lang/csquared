#include "csquare/lexer/lexer.h"
#include <stdbool.h>

token *lex_digit(const char *p, int *len, int *line, int *col) {
  const char *start = p;
  token_type type = T_DECIMAL;
  bool has_dot = false;
  bool has_exp = false;

  while (isdigit(*p))
    p++;

  if (*p == '.') {
    has_dot = true;
    p++;
    while (isdigit(*p))
      p++;
  }

  if (*p == 'e' || *p == 'E') {
    has_exp = true;
    p++;
    if (*p == '+' || *p == '-')
      p++;
    while (isdigit(*p))
      p++;
  }

  if (*p == 'u') {
    type = has_exp ? T_UNSIGNED_EXPO : T_UNSIGNED;
    p++;
  } else if (*p == 'f') {
    type = has_exp ? T_FLOAT_EXPO : T_FLOAT;
    p++;
  } else if (*p == 'q') {
    type = has_exp ? T_QUAD_EXPO : T_QUAD;
    p++;
  } else if (has_dot) {
    type = has_exp ? T_DOUBLE_EXPO : T_DOUBLE;
  } else if (!has_dot) {
    type = has_exp ? T_DECIMAL_EXPO : T_DECIMAL;
  }

  *len = (int)(p - start);
  *col = (*col) + (*len);
  return new_token(start, *len, type, *line, *col - *len);
}
