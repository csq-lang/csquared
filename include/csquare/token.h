/**
 * @file token.h
 * @brief Token type definitions and enumeration for C² lexer
 * @details Defines all token types recognized by the C² compiler,
 * including operators, keywords, literals, and punctuation. Also contains
 * the csq_token structure that represents a single token from source code.
 */

#ifndef C2_TOKEN_H
#define C2_TOKEN_H

#include <stddef.h>

#define TOKEN_LIST                                                             \
  X(TOKEN_EOF, "EOF")                                                          \
  X(TOKEN_ERROR, "ERROR")                                                      \
  X(TOKEN_IDENTIFIER, "IDENTIFIER")                                            \
  X(TOKEN_NUMBER, "NUMBER")                                                    \
  X(TOKEN_STRING, "STRING")                                                    \
  X(TOKEN_TAG, "TAG")                                                          \
  X(TOKEN_BOOLEAN, "BOOLEAN")                                                  \
  X(TOKEN_OPERATOR, "OPERATOR")                                                \
  X(TOKEN_PLUS, "PLUS")                                                        \
  X(TOKEN_MINUS, "MINUS")                                                      \
  X(TOKEN_STAR, "STAR")                                                        \
  X(TOKEN_SLASH, "SLASH")                                                      \
  X(TOKEN_PERCENT, "PERCENT")                                                  \
  X(TOKEN_CARET, "CARET")                                                      \
  X(TOKEN_AMPERSAND, "AMPERSAND")                                              \
  X(TOKEN_PIPE, "PIPE")                                                        \
  X(TOKEN_BANG, "BANG")                                                        \
  X(TOKEN_ASSIGN, "ASSIGN")                                                    \
  X(TOKEN_PLUS_ASSIGN, "PLUS_ASSIGN")                                          \
  X(TOKEN_MINUS_ASSIGN, "MINUS_ASSIGN")                                        \
  X(TOKEN_STAR_ASSIGN, "STAR_ASSIGN")                                          \
  X(TOKEN_SLASH_ASSIGN, "SLASH_ASSIGN")                                        \
  X(TOKEN_EQUAL, "EQUAL")                                                      \
  X(TOKEN_NOT_EQUAL, "NOT_EQUAL")                                              \
  X(TOKEN_LESS, "LESS")                                                        \
  X(TOKEN_GREATER, "GREATER")                                                  \
  X(TOKEN_LESS_EQUAL, "LESS_EQUAL")                                            \
  X(TOKEN_GREATER_EQUAL, "GREATER_EQUAL")                                      \
  X(TOKEN_LOGICAL_AND, "LOGICAL_AND")                                          \
  X(TOKEN_LOGICAL_OR, "LOGICAL_OR")                                            \
  X(TOKEN_INCREMENT, "INCREMENT")                                              \
  X(TOKEN_DECREMENT, "DECREMENT")                                              \
  X(TOKEN_DOUBLE_DOT, "DOUBLE_DOT")                                            \
  X(TOKEN_TRIPLE_DOT, "TRIPLE_DOT")                                            \
  X(TOKEN_RANGE, "RANGE")                                                      \
  X(TOKEN_ARROW, "ARROW")                                                      \
  X(TOKEN_OPEN_PAREN, "OPEN_PAREN")                                            \
  X(TOKEN_CLOSE_PAREN, "CLOSE_PAREN")                                          \
  X(TOKEN_OPEN_BRACE, "OPEN_BRACE")                                            \
  X(TOKEN_CLOSE_BRACE, "CLOSE_BRACE")                                          \
  X(TOKEN_OPEN_BRACKET, "OPEN_BRACKET")                                        \
  X(TOKEN_CLOSE_BRACKET, "CLOSE_BRACKET")                                      \
  X(TOKEN_COLON, "COLON")                                                      \
  X(TOKEN_SEMICOLON, "SEMICOLON")                                              \
  X(TOKEN_COMMA, "COMMA")                                                      \
  X(TOKEN_DOT, "DOT")                                                          \
  X(TOKEN_HASH, "HASH")                                                        \
  X(TOKEN_AT, "AT")                                                            \
  X(TOKEN_KEYWORD_FUNCTION, "KEYWORD_FUNCTION")                                \
  X(TOKEN_KEYWORD_FUNC, "KEYWORD_FUNC")                                        \
  X(TOKEN_KEYWORD_IF, "KEYWORD_IF")                                            \
  X(TOKEN_KEYWORD_ELSE, "KEYWORD_ELSE")                                        \
  X(TOKEN_KEYWORD_SWITCH, "KEYWORD_SWITCH")                                    \
  X(TOKEN_KEYWORD_CASE, "KEYWORD_CASE")                                        \
  X(TOKEN_KEYWORD_DEFAULT, "KEYWORD_DEFAULT")                                  \
  X(TOKEN_KEYWORD_WHILE, "KEYWORD_WHILE")                                      \
  X(TOKEN_KEYWORD_FOR, "KEYWORD_FOR")                                          \
  X(TOKEN_KEYWORD_IN, "KEYWORD_IN")                                            \
  X(TOKEN_KEYWORD_RETURN, "KEYWORD_RETURN")                                    \
  X(TOKEN_KEYWORD_THROW, "KEYWORD_THROW")                                      \
  X(TOKEN_KEYWORD_STRUCT, "KEYWORD_STRUCT")                                    \
  X(TOKEN_KEYWORD_ENUM, "KEYWORD_ENUM")                                        \
  X(TOKEN_KEYWORD_IMPORT, "KEYWORD_IMPORT")                                    \
  X(TOKEN_KEYWORD_NEW, "KEYWORD_NEW")                                          \
  X(TOKEN_KEYWORD_REPEAT, "KEYWORD_REPEAT")                                    \
  X(TOKEN_KEYWORD_UNTIL, "KEYWORD_UNTIL")                                      \
  X(TOKEN_KEYWORD_DEFER, "KEYWORD_DEFER")                                      \
  X(TOKEN_KEYWORD_TRY, "KEYWORD_TRY")                                          \
  X(TOKEN_KEYWORD_CATCH, "KEYWORD_CATCH")                                      \
  X(TOKEN_KEYWORD_SPAWN, "KEYWORD_SPAWN")                                      \
  X(TOKEN_KEYWORD_PRIVATE, "KEYWORD_PRIVATE")                                  \
  X(TOKEN_KEYWORD_SELF, "KEYWORD_SELF")                                        \
  X(TOKEN_KEYWORD_OR, "KEYWORD_OR")                                            \
  X(TOKEN_KEYWORD_AND, "KEYWORD_AND")                                          \
  X(TOKEN_KEYWORD_TRUE, "KEYWORD_TRUE")                                        \
  X(TOKEN_KEYWORD_FALSE, "KEYWORD_FALSE")                                      \
  X(TOKEN_KEYWORD_BOOL, "KEYWORD_BOOL")                                        \
  X(TOKEN_KEYWORD_INT, "KEYWORD_INT")                                          \
  X(TOKEN_KEYWORD_STRING, "KEYWORD_STRING")                                    \
  X(TOKEN_KEYWORD_FLOAT, "KEYWORD_FLOAT")                                      \
  X(TOKEN_KEYWORD_BREAK, "KEYWORD_BREAK")                                      \
  X(TOKEN_KEYWORD_CONTINUE, "KEYWORD_CONTINUE")

#define X(token, str) token,
typedef enum { TOKEN_LIST } csq_tktype;
#undef X

/**
 * @struct csq_token
 * @brief Represents a single token from the source code
 * @details A token is the basic unit produced by the lexer. It contains
 * the token type, the raw text from source, and location information.
 */
typedef struct {
  csq_tktype type;   /**< The type of token (keyword, operator, etc.) */
  const char *start; /**< Pointer to the start of token text in source */
  size_t length;     /**< Length of the token text in characters */
  size_t line;       /**< Line number where token appears (1-indexed) */
  size_t column;     /**< Column number where token appears (1-indexed) */
} csq_token;

extern const char *const token_strings[];

/**
 * @brief Convert a token type to its string representation
 * @param type The token type to convert
 * @return A string describing the token type (e.g., "IDENTIFIER", "PLUS")
 */
const char *token_type_to_string(csq_tktype type);

#endif
