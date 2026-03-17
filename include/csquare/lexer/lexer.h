#ifndef _LEXER_H
#define _LEXER_H

#include "csquare/error.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define T(NAME, STR) NAME,
#define TOKEN_TYPES                                                            \
  T(T_EOF, "EOF")                                                              \
  T(T_ERROR, "ERROR")                                                          \
  T(T_IDENTIFIER, "IDENTIFIER")                                                \
  T(T_DECIMAL, "DECIMAL")                                                      \
  T(T_UNSIGNED, "UNSIGNED DECIMAL")                                            \
  T(T_DOUBLE, "DOUBLE")                                                        \
  T(T_FLOAT, "FLOAT")                                                          \
  T(T_QUAD, "QUAD")                                                            \
  T(T_DECIMAL_EXPO, "DECIMAL EXPONENT")                                        \
  T(T_UNSIGNED_EXPO, "UNSIGNED DECIMAL EXPONENT")                              \
  T(T_DOUBLE_EXPO, "DOUBLE EXPONENT")                                          \
  T(T_FLOAT_EXPO, "FLOAT EXPONENT")                                            \
  T(T_QUAD_EXPO, "QUAD EXPONENT")                                              \
  T(T_STRING, "STRING")                                                        \
                                                                               \
  T(T_EQ, "EQUALS")                                                            \
  T(T_NEQ, "NOT EQUALS")                                                       \
  T(T_ASSIGN, "ASSIGN")                                                        \
  T(T_GREATER, "GREATER")                                                      \
  T(T_LESS, "LESS")                                                            \
  T(T_GREATER_EQUALS, "GREATER OR EQUALS")                                     \
  T(T_LESS_EQUALS, "LESS OR EQUALS")                                           \
  T(T_ADD, "ADD")                                                              \
  T(T_SUB, "SUBTRACT")                                                         \
  T(T_DIV, "DIVIDE")                                                           \
  T(T_MUL, "MULTIPLY")                                                         \
  T(T_ADD_ASSIGN, "ADD AND ASSIGN")                                            \
  T(T_SUB_ASSIGN, "SUBTRACT AND ASSIGN")                                       \
  T(T_DIV_ASSIGN, "DIVIDE AND ASSIGN")                                         \
  T(T_MUL_ASSIGN, "MULTIPLY AND ASSIGN")                                       \
  T(T_OPEN_PAREN, "OPEN PARENTHESES")                                          \
  T(T_CLOSE_PAREN, "CLOSE PARENTHESES")                                        \
  T(T_OPEN_BRACE, "OPEN BRACE")                                                \
  T(T_CLOSE_BRACE, "CLOSE BRACE")                                              \
  T(T_OPEN_BRACKET, "OPEN BRACKET")                                            \
  T(T_CLOSE_BRACKET, "CLOSE BRACKET")                                          \
  T(T_PERIOD, "PERIOD")                                                        \
  T(T_COMMA, "COMMA")                                                          \
  T(T_COLON, "COLON")                                                          \
  T(T_SEMICOLON, "SEMICOLON")                                                  \
  T(T_AND, "AND")                                                              \
  T(T_OR, "OR")                                                                \
  T(T_EXCLAMATION, "EXCLAMATION MARK")                                         \
                                                                               \
  T(T_KW_DO, "DO")                                                             \
  T(T_KW_IF, "IF")                                                             \
  T(T_KW_FOR, "FOR")                                                           \
  T(T_KW_INT, "INT")                                                           \
  T(T_KW_CHAR, "CHAR")                                                         \
  T(T_KW_VOID, "VOID")                                                         \
  T(T_KW_ELSE, "ELSE")                                                         \
  T(T_KW_ENUM, "ENUM")                                                         \
  T(T_KW_LONG, "LONG")                                                         \
  T(T_KW_QUAD, "QUAD")                                                         \
  T(T_KW_BOOL, "BOOL")                                                         \
  T(T_KW_CASE, "CASE")                                                         \
  T(T_KW_CONST, "CONST")                                                       \
  T(T_KW_TYPE, "TYPE")                                                         \
  T(T_KW_FLOAT, "FLOAT")                                                       \
  T(T_KW_GOTO, "GOTO")                                                         \
  T(T_KW_INFER, "INFER")                                                       \
  T(T_KW_SHORT, "SHORT")                                                       \
  T(T_KW_UCHAR, "UCHAR")                                                       \
  T(T_KW_UINT, "UINT")                                                         \
  T(T_KW_ULONG, "ULONG")                                                       \
  T(T_KW_ERROR, "ERROR")                                                       \
  T(T_KW_RETURN, "RETURN")                                                     \
  T(T_KW_STRUCT, "STRUCT")                                                     \
  T(T_KW_DOUBLE, "DOUBLE")                                                     \
  T(T_KW_STATIC, "STATIC")                                                     \
  T(T_KW_WHILE, "WHILE")                                                       \
  T(T_KW_DEFAULT, "DEFAULT")                                                   \
  T(T_KW_SWITCH, "SWITCH")                                                     \
  T(T_KW_USHORT, "USHORT")                                                     \
  T(T_KW_CONTINUE, "CONTINUE")

typedef enum { TOKEN_TYPES T__COUNT } token_type;

#undef T

extern const char *token_type_str[T__COUNT];

struct token {
  const char *start;
  int length;
  token_type type;

  int line;
  int col;
  error_type errtype;
  const char *errmsg;
};

typedef struct token token;

token *new_token(const char *start, int length, token_type type, int line,
                 int col);
void free_token(token *tk);
token *error_token(const char *msg, const char *src, int len, int line, int col,
                   error_type errtype);

typedef struct {
  token **tokens;
  size_t count;
  size_t capacity;
} token_list;

void init_token_list(token_list *list);
void free_token_list(token_list *list);
void add_token(token_list *list, token *tk);

#define isws(c) (c == ' ' || c == '\t' || c == '\n' || c == '\r')
#define isdigit(c) (c >= '0' && c <= '9')
#define isalpha(c) ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))

#define LEX_FUNC_ARGS const char *p, int *len, int *line, int *col
token *lex_symbol(LEX_FUNC_ARGS);
token *lex_digit(LEX_FUNC_ARGS);
token *lex_string(LEX_FUNC_ARGS);
token *lex_ident(LEX_FUNC_ARGS);
#undef LEX_FUNC_ARGS
token_list *lex(const char *src);

void print_token(token *tk);

#endif
