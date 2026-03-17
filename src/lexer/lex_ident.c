#include "csquare/lexer/lexer.h"

const struct {
  const char *kw;
  token_type type;
  int len;
} keyword_table[] = {{"do", T_KW_DO, 2},
                     {"if", T_KW_IF, 2},
                     {"for", T_KW_FOR, 3},
                     {"int", T_KW_INT, 3},
                     {"char", T_KW_CHAR, 4},
                     {"void", T_KW_VOID, 4},
                     {"else", T_KW_ELSE, 4},
                     {"enum", T_KW_ENUM, 4},
                     {"long", T_KW_LONG, 4},
                     {"quad", T_KW_QUAD, 4},
                     {"bool", T_KW_BOOL, 4},
                     {"case", T_KW_CASE, 4},
                     {"type", T_KW_TYPE, 4},
                     {"goto", T_KW_GOTO, 4},
                     {"uint", T_KW_UINT, 4},
                     {"const", T_KW_CONST, 5},
                     {"float", T_KW_FLOAT, 5},
                     {"infer", T_KW_INFER, 5},
                     {"short", T_KW_SHORT, 5},
                     {"uchar", T_KW_UCHAR, 5},
                     {"ulong", T_KW_ULONG, 5},
                     {"error", T_KW_ERROR, 5},
                     {"while", T_KW_WHILE, 5},
                     {"return", T_KW_RETURN, 6},
                     {"struct", T_KW_STRUCT, 6},
                     {"double", T_KW_DOUBLE, 6},
                     {"static", T_KW_STATIC, 6},
                     {"switch", T_KW_SWITCH, 6},
                     {"ushort", T_KW_USHORT, 6},
                     {"default", T_KW_DEFAULT, 7},
                     {"continue", T_KW_CONTINUE, 8}};

int keyword_count = sizeof(keyword_table) / sizeof(keyword_table[0]);

token *lex_ident(const char *p, int *len, int *line, int *col) {
  char buf[64];
  int bufi = 0;
  token_type type = T_IDENTIFIER;

  if (isalpha(*p) || *p == '_') {
    buf[bufi] = *p;
    bufi++;
    p++;
  }

  while ((isalpha(*p) || isdigit(*p) || *p == '_' || *p == '?') &&
         (size_t)bufi < sizeof(buf) - 1) {
    buf[bufi] = *p;
    bufi++;
    p++;
  }

  buf[bufi] = '\0';
  int skip_kw = (buf[bufi - 1] == '?');

  if (!skip_kw) {
    for (size_t i = 0; i < (size_t)keyword_count; i++) {
      if (bufi != keyword_table[i].len)
        continue;

      if (buf[0] != keyword_table[i].kw[0])
        continue;

      if (memcmp(buf, keyword_table[i].kw, bufi) == 0) {
        type = keyword_table[i].type;
        break;
      }
    }
  }

  const char *start = p - bufi;
  *len = bufi;
  *col = (*col) + (*len);
  return new_token(start, bufi, type, *line, *col - *len);
}
