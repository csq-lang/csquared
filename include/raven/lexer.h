#ifndef RAVEN_LEXER_H
#define RAVEN_LEXER_H

#include <core/diag.h>
#include <raven/source.h>
#include <raven/token.h>

typedef struct rvn_lexer {
  const char *buffer;
  const char *start;
  const char *current;
  size_t line;
  size_t column;
  const char *path;
  DiagReporter *diag;
} rvn_lexer;

typedef Token (*rvn_lexstate)(rvn_lexer *);

rvn_lexer *lexer_create(const rvn_source *source, DiagReporter *diag);
void lexer_free(rvn_lexer *lexer);
Token lexer_next(rvn_lexer *lexer);
void lexer_print_token(const Token *token);

Token lex_whitespace(rvn_lexer *lexer);
Token lex_identifier(rvn_lexer *lexer);
Token lex_number(rvn_lexer *lexer);
Token lex_string(rvn_lexer *lexer);
Token lex_tag(rvn_lexer *lexer);
Token lex_operator(rvn_lexer *lexer);

#endif
