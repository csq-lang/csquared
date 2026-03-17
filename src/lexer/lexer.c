#include "csquare/lexer/lexer.h"
#include "csquare/error.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#define T(NAME, STR) [NAME] = STR,

const char *token_type_str[T__COUNT] = {TOKEN_TYPES};

token *new_token(const char *start, int length, token_type type, int line,
                 int col) {
  token *tk = malloc(sizeof(token));
  if (!tk) {
    perror("malloc failed");
    return NULL;
  }

  tk->start = start;
  tk->length = length;
  tk->type = type;
  tk->line = line;
  tk->col = col;
  tk->errtype = ERROR_NONE;
  tk->errmsg = "";
  return tk;
}

void free_token(token *tk) {
  if (!tk)
    return;
  // if (tk->type == T_ERROR && tk->errmsg)
  //   free((void *)tk->errmsg);
  free(tk);
}

token *error_token(const char *msg, const char *src, int len, int line, int col,
                   error_type errtype) {
  token *tk = new_token(src, len, T_ERROR, line, col);
  tk->errtype = errtype;
  tk->errmsg = msg;
  return tk;
}

void free_token_list(token_list *list) {
  if (!list)
    return;

  for (size_t i = 0; i < list->count; i++) {
    if (list->tokens[i])
      free_token(list->tokens[i]);
  }
}

#define peek(n) (p[n])

void init_token_list(token_list *list) {
  list->count = 0;
  list->capacity = 32;
  list->tokens = malloc(sizeof(token *) * list->capacity);
}

void add_token(token_list *l, token *tok) {
  if (l->count >= l->capacity) {
    l->capacity *= 2;
    l->tokens = realloc(l->tokens, sizeof(token *) * l->capacity);
  }

  l->tokens[l->count++] = tok;
}

token_list *lex(const char *src) {
  token_list *list = malloc(sizeof(token_list));
  init_token_list(list);

  char *p = (char *)src;

  int line = 1;
  int col = 1;
  while (*p) {
    char c = *p;
    col++;

    if (isws(c)) {
      if (c == '\n' /*  || c == '\r'*/) {
        line++;
        col = 1;
      }
      p++;
      continue;
    }

    if (c == '/' && p[1] == '/') {
      p += 2;
      while (*p != '\n') {
        p++;
      }
      continue;
    }

    if (c == '/' && p[1] == '*') {
      p += 2;
      while (*p != '*' && *(p + 1) != '/') {
        p++;
      }
      p += 2;
      continue;
    }

    int consumed = 0;
    const char *msg = "unknown character \x1b[32m'%c'\x1b[0m";
    char buf[32];
    sprintf(buf, msg, c);
    token *tk;
    token *errtk =
        error_token(strdup(buf), p, 1, line, col - 1, SYNERR_UNKNOWN_CHARACTER);

    if (isdigit(c)) {
      tk = lex_digit(p, &consumed, &line, &col);
    } else if (isalpha(c) || c == '_') {
      tk = lex_ident(p, &consumed, &line, &col);
    } else if (c == '"' || c == '\'') {
      tk = lex_string(p, &consumed, &line, &col);
    } else {
      tk = lex_symbol(p, &consumed, &line, &col);
    }
    if (!tk)
      tk = errtk;

    p += consumed;
    add_token(list, tk);
  }

  add_token(list, new_token(p, 0, T_EOF, line, col));
  return list;
}

void print_token(token *tk) {
  const char *type_color = "\x1b[32m";
  int print_errmsg = 0;
  if (tk->type == T_ERROR) {
    type_color = "\x1b[31m";
    print_errmsg = 1;
  }

  printf("Text: \x1b[33m");

  for (int i = 0; i < tk->length; i++) {
    char c = tk->start[i];
    if (c == '\n')
      printf("\x1b[36m\\n\x1b[0m");
    else
      putchar(c);
  }

  printf("\x1b[0m, Type: %s%s\x1b[0m", type_color, token_type_str[tk->type]);
  if (print_errmsg) {
    printf(", Error message: \x1b[31m%s\x1b[0m", tk->errmsg);
  }
  printf("\n");
}
