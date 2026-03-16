#include "csquare/lexer/lexer.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#define T(NAME, STR) [NAME] = STR,

const char *token_type_str[T__COUNT] = {TOKEN_TYPES};

token *new_token(const char *start, int length, token_type type) {
  token *tk = malloc(sizeof(token));
  if (!tk) {
    perror("malloc failed");
    return NULL;
  }

  tk->start = strdup(start);
  tk->length = length;
  tk->type = type;
  return tk;
}

void free_token(token *tk) {
  if (!tk)
    return;

  free(tk);
}

token *emit(const char *src, int starti, int endi, token_type type) {
  if (starti < 0 || endi < starti) {
    return NULL;
  }

  int len = endi - starti;
  char *start = malloc(len + 1);
  if (!start) {
    return NULL;
  }

  for (int i = 0; i < len; i++) {
    start[i] = src[starti + i];
  }
  start[len] = '\0';

  return new_token(start, len, type);
}

token *error_token(const char *msg) {
  return new_token(msg, strlen(msg), T_ERROR);
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

  while (*p) {
    char c = *p;

    if (isws(c)) {
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
    token *tk = NULL;

    if (isdigit(c)) {
      tk = lex_digit(p, &consumed);
    } else if (isalpha(c) || c == '_') {
      tk = lex_ident(p, &consumed);
    } else if (c == '"' || c == '\'') {
      tk = lex_string(p, &consumed);
    } else {
      tk = lex_symbol(p, &consumed);
    }

    p += consumed;
    add_token(list, tk);
  }

  return list;
}

void print_token(token *tk) {
  const char *type_color = "\x1b[32m";
  if (tk->type == T_ERROR)
    type_color = "\x1b[31m";

  printf("Text: \x1b[33m");

  for (int i = 0; i < tk->length; i++) {
    char c = tk->start[i];
    if (c == '\n')
      printf("\x1b[36m\\n\x1b[0m");
    else
      putchar(c);
  }

  printf("\x1b[0m, Type: %s%s\x1b[0m\n", type_color, token_type_str[tk->type]);
}
