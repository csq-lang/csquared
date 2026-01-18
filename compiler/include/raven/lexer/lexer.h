#ifndef INCLUDE_INCLUDE_LEXER_H_
#define INCLUDE_INCLUDE_LEXER_H_

#define TOKEN_MAX 0xFFFFF

typedef enum {
  T_EOF,
  T_STRING,
  T_STR_INTER,
  T_NUMBER,
  T_ID,
  T_TAG,

  T_FUNCTION,
  T_IF,
  T_ELSE,
  T_SWITCH,
  T_CASE,
  T_DEFAULT,
  T_WHILE,
  T_FOR,
  T_RETURN,
  T_THROW,
  T_STRUCT,
  T_ENUM,
  T_CONST,
  T_LET,
  T_IN,
  T_IMPORT,
  T_NEW,
  T_REPEAT,
  T_UNTIL,
  T_DEFER,
  T_TRY,
  T_CATCH,
  T_SPAWN,
  T_PRIVATE,
  T_SELF,
  T_OR,
  T_AND,
  T_TRUE,
  T_FALSE,

  T_PUNCTUATION,
  T_OPERATOR,

  T_DECORATOR
} token_type_t;

typedef struct {
  const char *word;
  token_type_t assigned_type;
} keyword_t;

typedef struct {
  unsigned long line;
  unsigned long col;
} tkpos_t;

typedef struct {
  char *txt;
  token_type_t type;
  tkpos_t position;
} token_t;

typedef struct {
  int token_num;
  token_t *tokens[TOKEN_MAX];
} token_list_t;

token_list_t *lex(const char *source);

token_t *new_token(const char *txt, token_type_t type, unsigned long line,
                   unsigned long col);
void free_token(token_t *token);

token_list_t *new_token_list();
void free_token_list(token_list_t *token_list);
void append_to_token_list(token_list_t *list, token_t *token);

void _display_token(token_t *token);
void _display_token_list(token_list_t *list);

#endif // INCLUDE_INCLUDE_LEXER_H_
