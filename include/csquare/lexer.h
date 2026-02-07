/**
 * @file lexer.h
 * @brief Lexical analyzer (tokenizer) for C² source code
 * @details The lexer converts raw source code into a stream of tokens.
 * It handles whitespace, identifiers, numbers, strings, keywords, and operators.
 */

#ifndef C2_LEXER_H
#define C2_LEXER_H

#include <csquare/source.h>
#include <csquare/token.h>
#include <core/diag.h>

/**
 * @struct csq_lexer
 * @brief Lexer state and position tracking
 * @details Maintains position in source code and diagnostic information.
 * Used internally to parse the source into tokens.
 */
typedef struct csq_lexer {
    const char* buffer;     /**< Entire source file buffer */
    const char* start;      /**< Start of current token being scanned */
    const char* current;    /**< Current position in buffer */
    size_t line;            /**< Current line number (1-indexed) */
    size_t column;          /**< Current column number (1-indexed) */
    const char* path;       /**< Path to source file for error messages */
    DiagReporter* diag;     /**< Diagnostic reporter for error handling */
} csq_lexer;

/**
 * @brief Function pointer type for lexer state handlers
 * @param lexer The lexer instance
 * @return The token produced by this state
 * @details Used internally for state machine-based lexing
 */
typedef csq_token (*csq_lexstate)(csq_lexer*);

/**
 * @brief Create a new lexer instance
 * @param source The source file to tokenize
 * @param diag Diagnostic reporter for error handling
 * @return Pointer to new csq_lexer, or NULL on error
 * @details Initializes the lexer and jump table for efficient token recognition.
 */
csq_lexer* lexer_create(const csq_source* source, DiagReporter* diag);

/**
 * @brief Free lexer memory
 * @param lexer Pointer to lexer to free
 * @details Must be called when lexer is no longer needed.
 */
void lexer_free(csq_lexer* lexer);

/**
 * @brief Get the next token from the source
 * @param lexer The lexer instance
 * @return The next token from source code
 * @details Returns TOKEN_EOF when end of file is reached.
 */
csq_token lexer_next(csq_lexer* lexer);

/**
 * @brief Print a token for debugging
 * @param token The token to print
 * @details Outputs token type and content to stdout.
 */
void lexer_print_token(const csq_token* token);

/**
 * @brief Tokenize whitespace
 * @param lexer The lexer instance
 * @return Whitespace token or token from next state
 * @internal Used by lexer state machine
 */
csq_token lex_whitespace(csq_lexer* lexer);

/**
 * @brief Tokenize identifiers and keywords
 * @param lexer The lexer instance
 * @return Identifier or keyword token
 * @internal Used by lexer state machine
 */
csq_token lex_identifier(csq_lexer* lexer);

/**
 * @brief Tokenize numeric literals
 * @param lexer The lexer instance
 * @return Number token (integer or float)
 * @internal Used by lexer state machine
 */
csq_token lex_number(csq_lexer* lexer);

/**
 * @brief Tokenize string literals
 * @param lexer The lexer instance
 * @return String token
 * @internal Used by lexer state machine
 */
csq_token lex_string(csq_lexer* lexer);

/**
 * @brief Tokenize tag literals
 * @param lexer The lexer instance
 * @return Tag token
 * @internal Used by lexer state machine
 */
csq_token lex_tag(csq_lexer* lexer);

/**
 * @brief Tokenize operators and punctuation
 * @param lexer The lexer instance
 * @return Operator token
 * @internal Used by lexer state machine
 */
csq_token lex_operator(csq_lexer* lexer);

/**
 * @brief Peek at current character without consuming
 * @param lexer The lexer instance
 * @return Current character, or '\0' at EOF
 * @details Does not advance lexer position.
 */
char peek(csq_lexer* lexer);

/**
 * @brief Peek at next character without consuming
 * @param lexer The lexer instance
 * @return Next character, or '\0' at EOF
 * @details Does not advance lexer position.
 */
char peek_next(csq_lexer* lexer);

/**
 * @brief Consume current character and move to next
 * @param lexer The lexer instance
 * @details Updates line and column numbers. Called to progress through source.
 */
void advance(csq_lexer* lexer);

#endif