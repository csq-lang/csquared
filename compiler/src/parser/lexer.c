#include "lexer.h"
#include "colors.h"
#include "rvnerror.h"
#include "rvnutils.h"

#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NEXT_C_OUT_OF_BOUNDS (i + 1 >= source_len)
#define SKIP_CHAR                                                              \
  i++;                                                                         \
  c = source[i];
#define SKIP_CHAR_IF_INBOUNDS                                                  \
  if (!NEXT_C_OUT_OF_BOUNDS) {                                                 \
    /*printf("Skipping char '%c'\n", c);*/                                     \
    SKIP_CHAR                                                                  \
  } else {                                                                     \
    THROW("Out of bounds when lexing. Expected more characters.");             \
  }

token_t *new_token(const char *txt, token_type_t type, unsigned long line,
                   unsigned long col) {
  token_t *token = malloc(sizeof(token_t));
  token->txt = (char *)txt;
  token->type = type;
  tkpos_t pos = {line, col};
  token->position = pos;
  return token;
}

void free_token(token_t *token) {
  if (token && token->type != T_EOF) {
    if (token->txt) {
      free(token->txt);
    }
    free(token);
  }
}

token_list_t *new_token_list() {
  token_list_t *token_list = malloc(sizeof(token_list_t));
  token_list->token_num = 0;
  return token_list;
}

void free_token_list(token_list_t *token_list) {
  if (token_list) {
    for (int i = 0; i < token_list->token_num; i++) {
      free_token(token_list->tokens[i]);
    }
    free(token_list);
  }
}

void append_to_token_list(token_list_t *list, token_t *token) {
  if (list->token_num >= TOKEN_MAX) {
    free_token(token);
    THROW("Token limit exceeded (%d tokens max)", TOKEN_MAX);
  }
  list->tokens[list->token_num] = token;
  list->token_num++;
  //_display_token(token);
}

int is_digit_char(char c) {
  if (c >= '0' && c <= '9') {
    return 1;
  }
  return 0;
}

int is_alpha_char(char c) {
  if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')) {
    return 1;
  }
  return 0;
}

int is_punct_char(char c) {
  if (c == '(' || c == ')' || c == '{' || c == '}' || c == '[' || c == ']' ||
      c == '.' || c == ',' || c == ';' || c == '"' || c == ':') {
    return 1;
  }
  return 0;
}

int is_op_char(char c) {
  if (c == '+' || c == '-' || c == '*' || c == '/' || c == '^' || c == '>' ||
      c == '<' || c == '=' || c == '!') {
    return 1;
  }
  return 0;
}

const keyword_t keywords[28] = {
    {"function", T_FUNCTION}, {"if", T_IF},         {"else", T_ELSE},
    {"switch", T_SWITCH},     {"case", T_CASE},     {"default", T_DEFAULT},
    {"while", T_WHILE},       {"for", T_FOR},       {"return", T_RETURN},
    {"throw", T_THROW},       {"struct", T_STRUCT}, {"enum", T_ENUM},
    {"const", T_CONST},       {"let", T_LET},       {"in", T_IN},
    {"import", T_IMPORT},     {"new", T_NEW},       {"repeat", T_REPEAT},
    {"until", T_UNTIL},       {"defer", T_DEFER},   {"try", T_TRY},
    {"catch", T_CATCH},       {"spawn", T_SPAWN},   {"private", T_PRIVATE},
    {"self", T_SELF},         {"or", T_OR},         {"and", T_AND}};

token_list_t *lex(const char *source) {
  int source_len = strlen(source);
  token_list_t *tokens = new_token_list();
  char c;
  unsigned long line = 1;
  unsigned long col = 1;

  for (int i = 0; i < source_len; i++) {
    c = source[i];
    col++;

    if (c == '/' && !NEXT_C_OUT_OF_BOUNDS && source[i + 1] == '/') {
      while (c != '\n' && c != '\r') {
        SKIP_CHAR_IF_INBOUNDS;
      }
      line++;
      continue;
    }

    if (c == ' ' || c == '\n' || c == '\t' || c == '\r') {
      if (c == '\n') {
        line++;
        col = 1;
        goto new_line;
      }
    new_line:
      continue;
    }

    if (is_digit_char(c)) {
      char *num = calloc(source_len + 1, 1);
      num[0] = c;
      SKIP_CHAR_IF_INBOUNDS;

      while (is_digit_char(c) == 1) {
        strcatchr(num, c);
        SKIP_CHAR_IF_INBOUNDS;
      }
      append_to_token_list(tokens, new_token(num, T_NUMBER, line, col));

      continue;
    }

    if (c == '"' || c == '\'') {
      char end = c;
      SKIP_CHAR_IF_INBOUNDS;
      char *str = calloc(source_len + 1, 1);

      while (c != end) {
        if (c == '$' && !NEXT_C_OUT_OF_BOUNDS && source[i + 1] == '{') {
          SKIP_CHAR_IF_INBOUNDS;
          SKIP_CHAR_IF_INBOUNDS;
          char *interpol = calloc(source_len + 1, 1);
          while (c != '}') {
            strcatchr(interpol, c);
            SKIP_CHAR_IF_INBOUNDS;
          }
          SKIP_CHAR_IF_INBOUNDS;
          append_to_token_list(tokens,
                               new_token(interpol, T_STR_INTER, line, col));
          if (c == end) {
            break;
          }
        }
        strcatchr(str, c);
        SKIP_CHAR_IF_INBOUNDS;
      }
      append_to_token_list(tokens, new_token(str, T_STRING, line, col));
      continue;
    }

    if (is_punct_char(c)) {
      char *punct = calloc(source_len + 1, 1);
      strcatchr(punct, c);
      append_to_token_list(tokens, new_token(punct, T_PUNCTUATION, line, col));
      continue;
    }

    if (is_op_char(c)) {
      char *op = calloc(source_len + 1, 1);
      strcatchr(op, c);
      SKIP_CHAR_IF_INBOUNDS;
      if ((op[0] == '>' && c == '=') || (op[0] == '<' && c == '=') ||
          (op[0] == '!' && c == '=') || (op[0] == '=' && c == '=')) {
        strcatchr(op, c);
      }
      append_to_token_list(tokens, new_token(op, T_OPERATOR, line, col));
      continue;
    }

    if (is_alpha_char(c) || c == '_') {
      char *id = calloc(source_len + 1, 1);

      while (is_alpha_char(c) || is_digit_char(c) || c == '_') {
        strcatchr(id, c);
        SKIP_CHAR_IF_INBOUNDS;
      }

      for (size_t j = 0; j <= 26; j++) {
        if (strcmp(keywords[j].word, id) == 0) {
          append_to_token_list(
              tokens, new_token(id, keywords[j].assigned_type, line, col));
          i--;
          goto was_reserved;
        }
      }

      i--;
      append_to_token_list(tokens, new_token(id, T_ID, line, col));
    was_reserved:
      continue;
    }

    if (c == '#') {
      char *deco = calloc(source_len + 1, 1);
      SKIP_CHAR_IF_INBOUNDS;
      while (c != '\n' && c != '\r') {
        strcatchr(deco, c);
        SKIP_CHAR_IF_INBOUNDS;
      }
      append_to_token_list(tokens, new_token(deco, T_DECORATOR, line, col));
      continue;
    }

    if (c == '@') {
      SKIP_CHAR_IF_INBOUNDS;
      if (c == '"' || c == '\'') {
        char end = c;
        SKIP_CHAR_IF_INBOUNDS;
        char *tag = calloc(source_len + 1, 1);

        while (c != end) {
          strcatchr(tag, c);
          SKIP_CHAR_IF_INBOUNDS;
        }
        append_to_token_list(tokens, new_token(tag, T_TAG, line, col));
        continue;
      }
    }

    THROW("Unrecognized character '%c' at line %lu, column %lu\n", c, line,
          col);
  }

  char eof[2] = {'\0'};
  append_to_token_list(tokens, new_token(eof, T_EOF, line, col));
  return tokens;
}

void _display_token(token_t *token) {
  char *type_str = "";
  switch (token->type) {
  case T_EOF:
    type_str = "EOF";
    break;
  case T_STRING:
    type_str = "String";
    break;
  case T_NUMBER:
    type_str = "Number";
    break;
  case T_ID:
    type_str = "Identifier";
    break;
  case T_STR_INTER:
    type_str = "String interpolation";
    break;
  case T_TAG:
    type_str = "Tag";
    break;
  case T_PUNCTUATION:
    type_str = "Punctuation";
    break;
  case T_OPERATOR:
    type_str = "Operator";
    break;
  case T_DECORATOR:
    type_str = "Decorator";
    break;

  // P.S.: all cases below were made with a Vim macro, don't worry about my
  // fingers/sanity.
  case T_FUNCTION:
    type_str = "Function";
    break;
  case T_IF:
    type_str = "If";
    break;
  case T_ELSE:
    type_str = "Else";
    break;
  case T_SWITCH:
    type_str = "Switch";
    break;
  case T_CASE:
    type_str = "Case";
    break;
  case T_DEFAULT:
    type_str = "Default";
    break;
  case T_WHILE:
    type_str = "While";
    break;
  case T_FOR:
    type_str = "For";
    break;
  case T_RETURN:
    type_str = "Return";
    break;
  case T_THROW:
    type_str = "Throw";
    break;
  case T_STRUCT:
    type_str = "Struct";
    break;
  case T_ENUM:
    type_str = "Enum";
    break;
  case T_CONST:
    type_str = "Const";
    break;
  case T_LET:
    type_str = "Let";
    break;
  case T_IN:
    type_str = "In";
    break;
  case T_IMPORT:
    type_str = "Import";
    break;
  case T_NEW:
    type_str = "New";
    break;
  case T_REPEAT:
    type_str = "Repeat";
    break;
  case T_UNTIL:
    type_str = "Until";
    break;
  case T_DEFER:
    type_str = "Defer";
    break;
  case T_TRY:
    type_str = "Try";
    break;
  case T_CATCH:
    type_str = "Catch";
    break;
    break;
  case T_SPAWN:
    type_str = "Spawn";
    break;
    break;
  case T_PRIVATE:
    type_str = "Private";
    break;
  case T_SELF:
    type_str = "Self";
    break;
  case T_OR:
    type_str = "Or";
    break;
  case T_AND:
    type_str = "And";
    break;
  }
  printf("Text: " GRN "%s" CRESET ", type: " BLU "%s" CRESET ", position: " MAG
         "%lu, %lu" CRESET "\n",
         token->txt, type_str, token->position.line, token->position.col);
}

void _display_token_list(token_list_t *list) {
  for (int i = 0; i < list->token_num; i++) {
    printf(RED "===== Token %d =====" CRESET "\n", i);
    _display_token(list->tokens[i]);
  }
}
