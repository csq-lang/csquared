#include <raven/token.h>

static const char *token_strings[] = {"EOF",
                                      "ERROR",
                                      "IDENTIFIER",
                                      "NUMBER",
                                      "STRING",
                                      "TAG",
                                      "BOOLEAN",

                                      "OPERATOR",
                                      "PLUS",
                                      "MINUS",
                                      "STAR",
                                      "SLASH",
                                      "PERCENT",
                                      "CARET",

                                      "ASSIGN",
                                      "PLUS_ASSIGN",
                                      "MINUS_ASSIGN",
                                      "STAR_ASSIGN",
                                      "SLASH_ASSIGN",

                                      "EQUAL",
                                      "NOT_EQUAL",
                                      "LESS",
                                      "GREATER",
                                      "LESS_EQUAL",
                                      "GREATER_EQUAL",

                                      "LOGICAL_AND",
                                      "LOGICAL_OR",

                                      "INCREMENT",
                                      "DECREMENT",
                                      "DOUBLE_DOT",
                                      "TRIPLE_DOT",
                                      "RANGE",
                                      "ARROW",

                                      "OPEN_PAREN",
                                      "CLOSE_PAREN",
                                      "OPEN_BRACE",
                                      "CLOSE_BRACE",
                                      "OPEN_BRACKET",
                                      "CLOSE_BRACKET",

                                      "COLON",
                                      "SEMICOLON",
                                      "COMMA",
                                      "DOT",
                                      "HASH",
                                      "AT",

                                      "KEYWORD_FUNCTION",
                                      "KEYWORD_FUNC",
                                      "KEYWORD_IF",
                                      "KEYWORD_ELSE",
                                      "KEYWORD_SWITCH",
                                      "KEYWORD_CASE",
                                      "KEYWORD_DEFAULT",
                                      "KEYWORD_WHILE",
                                      "KEYWORD_FOR",
                                      "KEYWORD_IN",
                                      "KEYWORD_RETURN",
                                      "KEYWORD_THROW",
                                      "KEYWORD_STRUCT",
                                      "KEYWORD_ENUM",
                                      "KEYWORD_IMPORT",
                                      "KEYWORD_NEW",
                                      "KEYWORD_REPEAT",
                                      "KEYWORD_UNTIL",
                                      "KEYWORD_DEFER",
                                      "KEYWORD_TRY",
                                      "KEYWORD_CATCH",
                                      "KEYWORD_SPAWN",
                                      "KEYWORD_PRIVATE",
                                      "KEYWORD_SELF",
                                      "KEYWORD_OR",
                                      "KEYWORD_AND",
                                      "KEYWORD_TRUE",
                                      "KEYWORD_FALSE",

                                      "KEYWORD_INT",
                                      "KEYWORD_STRING",
                                      "KEYWORD_FLOAT"};

const char *token_type_to_string(TokenType type) {
  if (type < sizeof(token_strings) / sizeof(token_strings[0])) {
    return token_strings[type];
  }
  return "UNKNOWN";
}
