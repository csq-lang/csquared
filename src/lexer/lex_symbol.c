#include "csquare/lexer/lexer.h"

const struct {
  const char *sym;
  token_type type;
} symbol_table[] = {
    {"==", T_EQ},          {"!=", T_NEQ},          {">=", T_GREATER_EQUALS},
    {"<=", T_LESS_EQUALS}, {"+=", T_ADD_ASSIGN},   {"-=", T_SUB_ASSIGN},
    {"/=", T_DIV_ASSIGN},  {"*=", T_MUL_ASSIGN},   {"&&", T_AND},
    {"||", T_OR},

    {"=", T_ASSIGN},       {"+", T_ADD},           {"-", T_SUB},
    {"/", T_DIV},          {"*", T_MUL},           {">", T_GREATER},
    {"<", T_LESS},         {"!", T_EXCLAMATION},   {"(", T_OPEN_PAREN},
    {")", T_CLOSE_PAREN},  {"{", T_OPEN_BRACE},    {"}", T_CLOSE_BRACE},
    {"[", T_OPEN_BRACKET}, {"]", T_CLOSE_BRACKET}, {".", T_PERIOD},
    {",", T_COMMA},        {":", T_COLON},         {";", T_SEMICOLON}};

int symbol_count = sizeof(symbol_table) / sizeof(symbol_table[0]);

token *lex_symbol(const char *p, int *len, int *line, int *col) {
  int best_len = 0;
  token_type best_type = T_ERROR;

  for (int i = 0; i < symbol_count; i++) {
    int sym_len = strlen(symbol_table[i].sym);
    if (strncmp(p, symbol_table[i].sym, sym_len) == 0 && sym_len > best_len) {
      best_len = sym_len;
      best_type = symbol_table[i].type;
    }
  }

  if (best_len == 0) {
    best_len = 1;
    best_type = T_ERROR;
    *len = best_len;
    return NULL;
  }

  *len = best_len;
  *col = (*col) + (*len);
  return new_token(p, best_len, best_type, *line, *col - *len);
}
