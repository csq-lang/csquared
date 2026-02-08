/**
 * @file lexer.c
 * @brief Lexical analyzer for the C² compiler
 * @details Implements tokenization of C² source code, handling identifiers,
 * keywords, operators, literals, and various token types. Uses a state machine
 * approach for efficient token recognition.
 */

#include <csquare/jmptable.h>
#include <csquare/keywrd.h>
#include <csquare/lexer.h>
#include <csquare/logger.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <util/prefix.h>

/**
 * @brief Create a new lexer instance
 * @details Initializes a lexer with the provided source buffer and diagnostic
 * reporter. Sets up initial state for tokenization starting from the beginning
 * of the buffer.
 * @param source Pointer to source file structure containing the buffer
 * @param diag Diagnostic reporter for error handling
 * @return Newly allocated lexer instance, or NULL on allocation failure
 */
csq_lexer *lexer_create(const csq_source *source, DiagReporter *diag) {
  /* Initialize the jump table */
  static bool initialized = false;
  if (!initialized) {
    initialize_state_table();
    initialized = true;
  }
  // Allocate the memory for the csq_lexer depending of it's size
  csq_lexer *lexer = malloc(sizeof(csq_lexer));
  if (!lexer) {
    RVN_FATAL("Not enough memory to create lexer");
    return NULL;
  }
  // Some initializations
  lexer->buffer = source->buffer;
  lexer->start = source->buffer;
  lexer->current = source->buffer;
  lexer->line = 1;
  lexer->column = 1;
  lexer->path = source->path;
  lexer->diag = diag;

  return lexer;
}
/**
 * @brief Free lexer resources
 * @param lexer Lexer instance to deallocate
 */
void lexer_free(csq_lexer *lexer) { free(lexer); }
/**
 * @brief Advance lexer position to next character
 * @details Updates line and column counters appropriately when encountering
 * newlines.
 * @param lexer Lexer instance
 */
void advance(csq_lexer *lexer) {
  if (*lexer->current == '\n') {
    lexer->line++;
    lexer->column = 1;
  } else
    lexer->column++;
  lexer->current++;
}

/**
 * @brief Peek at current character without advancing
 * @param lexer Lexer instance
 * @return Current character
 */
char peek(csq_lexer *lexer) { return *lexer->current; }
/**
 * @brief Peek at the next character without advancing
 * @param lexer Lexer instance
 * @return Next character or null terminator if at end
 */
char peek_next(csq_lexer *lexer) {
  return *lexer->current ? *(lexer->current + 1) : '\0';
}
/**
 * @brief Create a token from current lexer state
 * @param lexer Lexer instance
 * @param type Token type
 * @param line Line number of token start
 * @param column Column number of token start
 * @return Newly created token
 */
csq_token make_token(csq_lexer *lexer, csq_tktype type, size_t line,
                     size_t column) {
  return (csq_token){.type = type,
                     .start = lexer->start,
                     .length = (size_t)(lexer->current - lexer->start),
                     .line = line,
                     .column = column};
}

/**
 * @brief Report a lexical error through the diagnostic system
 * @param lexer Lexer instance
 * @param type Error type enumeration
 * @param start_col Starting column of error
 * @param length Length of error region
 * @param message Error message
 */
void report_error(csq_lexer *lexer, DiagErrorType type, size_t start_col,
                  size_t length, const char *message) {
  if (lexer->diag) {
    diag_report(lexer->diag, type, lexer->path, lexer->line, start_col, length,
                message);
  }
}

/**
 * @brief Skip whitespace and return next token
 * @param lexer Lexer instance
 * @return Next non-whitespace token
 */
csq_token lex_whitespace(csq_lexer *lexer) { return lexer_next(lexer); }
/**
 * @brief Compare keyword entry with string
 * @param kw Keyword entry to compare
 * @param str String to compare against
 * @param len Length of string
 * @return Comparison result (0 if equal)
 */
static inline int kw_compare(const struct keyword_entry *kw, const char *str,
                             size_t len) {
  if (kw->length != len)
    return (int)kw->length - (int)len;
  return memcmp(kw->text, str, len);
}
/**
 * @brief Check if identifier is a keyword using binary search
 * @param start Pointer to identifier string
 * @param length Length of identifier
 * @return Token type if keyword, TOKEN_IDENTIFIER otherwise
 */
static csq_tktype check_keyword(const char *start, size_t length) {
  int left = 0, right = KEYWORDS_COUNT - 1;

  while (left <= right) {
    int mid = left + ((right - left) >> 1);
    int cmp = kw_compare(&keywords[mid], start, length);

    if (cmp == 0)
      return keywords[mid].type;
    if (cmp < 0)
      left = mid + 1;
    else
      right = mid - 1;
  }
  return TOKEN_IDENTIFIER;
}
/**
 * @brief Tokenize an identifier or keyword
 * @details Reads an identifier-like token and determines if it's a keyword or
 * identifier.
 * @param lexer Lexer instance
 * @return Token representing identifier or keyword
 */
csq_token lex_identifier(csq_lexer *lexer) {
  size_t start_col = lexer->column;

  while (isalnum((unsigned char)*lexer->current) || *lexer->current == '_') {
    advance(lexer);
  }

  size_t length = (size_t)(lexer->current - lexer->start);
  csq_tktype type = check_keyword(lexer->start, length);

  if (type == TOKEN_IDENTIFIER) {
    if (length > 0 && isdigit((unsigned char)lexer->start[0])) {
      report_error(lexer, DIAG_ERROR_INVALID_IDENTIFIER, start_col, length,
                   "identifier cannot start with a digit");
      return make_token(lexer, TOKEN_ERROR, lexer->line, start_col);
    }
  }

  return make_token(lexer, type, lexer->line, start_col);
}
/**
 * @brief Tokenize a numeric literal
 * @details Handles decimal, hex (0x), binary (0b), and octal (0o) number
 * formats, including floating-point numbers.
 * @param lexer Lexer instance
 * @return Token representing numeric literal
 */
csq_token lex_number(csq_lexer *lexer) {
  size_t start_col = lexer->column;
  int base = 10;

  if (*lexer->current == '0') {
    char next = peek_next(lexer);
    if (is_valid_base_prefix(*lexer->current, next)) {
      advance(lexer);
      advance(lexer);
      switch (next) {
      case 'x':
      case 'X':
        base = 16;
        break;
      case 'b':
      case 'B':
        base = 2;
        break;
      case 'o':
      case 'O':
        base = 8;
        break;
      }

      if (!is_digit_in_base(peek(lexer), base)) {
        report_error(lexer, DIAG_ERROR_INVALID_BASE, start_col, 2,
                     "invalid digit for specified number base");
        return make_token(lexer, TOKEN_ERROR, lexer->line, start_col);
      }
    }
  }

  bool has_digits = false;
  while (is_digit_in_base(peek(lexer), base)) {
    advance(lexer);
    has_digits = true;
  }

  if (!has_digits && base != 10) {
    report_error(lexer, DIAG_ERROR_MALFORMED_NUMBER, start_col,
                 (size_t)(lexer->current - lexer->start),
                 "malformed numeric literal");
    return make_token(lexer, TOKEN_ERROR, lexer->line, start_col);
  }

  if (base == 10 && peek(lexer) == '.') {
    advance(lexer);
    bool has_frac_digits = false;
    while (isdigit((unsigned char)peek(lexer))) {
      advance(lexer);
      has_frac_digits = true;
    }
    if (!has_frac_digits && !has_digits) {
      report_error(lexer, DIAG_ERROR_MALFORMED_NUMBER, start_col,
                   (size_t)(lexer->current - lexer->start),
                   "malformed floating-point number");
      return make_token(lexer, TOKEN_ERROR, lexer->line, start_col);
    }
  }

  if (isalpha((unsigned char)peek(lexer))) {
    report_error(lexer, DIAG_ERROR_MALFORMED_NUMBER, start_col,
                 (size_t)(lexer->current - lexer->start) + 1,
                 "invalid character in numeric literal");
    return make_token(lexer, TOKEN_ERROR, lexer->line, start_col);
  }

  return make_token(lexer, TOKEN_NUMBER, lexer->line, start_col);
}

/**
 * @brief Check if character is a valid escape sequence character
 * @param c Character to check
 * @return True if valid escape character (n, t, r, \, ", ', 0, x)
 */
static bool is_valid_escape(char c) {
  return c == 'n' || c == 't' || c == 'r' || c == '\\' || c == '"' ||
         c == '\'' || c == '0' || c == 'x';
}
/**
 * @brief Scan a quoted string or character literal
 * @details Handles escape sequences, validates proper termination.
 * @param lexer Lexer instance
 * @param type Token type (STRING or TAG)
 * @return Token for the quoted literal
 */
static csq_token scan_quoted_literal(csq_lexer *lexer, csq_tktype type) {
  char quote = peek(lexer);
  size_t start_col = lexer->column;
  advance(lexer);

  while (peek(lexer) && peek(lexer) != quote) {
    if (peek(lexer) == '\\') {
      advance(lexer);
      char esc = peek(lexer);
      if (!esc) {
        report_error(lexer, DIAG_ERROR_UNTERMINATED_STRING, start_col,
                     (size_t)(lexer->current - lexer->start),
                     "unterminated string literal");
        return make_token(lexer, TOKEN_ERROR, lexer->line, start_col);
      }
      if (!is_valid_escape(esc)) {
        size_t err_col = lexer->column;
        report_error(lexer, DIAG_ERROR_INVALID_ESCAPE, err_col, 2,
                     "invalid escape sequence");
      }
      if (esc == 'x') {
        advance(lexer);
        if (!isxdigit((unsigned char)peek(lexer))) {
          report_error(lexer, DIAG_ERROR_INVALID_ESCAPE, lexer->column, 1,
                       "invalid hex escape sequence");
          advance(lexer);
        } else {
          advance(lexer);
          if (isxdigit((unsigned char)peek(lexer))) {
            advance(lexer);
          }
        }
      } else {
        advance(lexer);
      }
    } else if (peek(lexer) == '\n' || peek(lexer) == '\r') {
      report_error(lexer, DIAG_ERROR_UNTERMINATED_STRING, start_col,
                   (size_t)(lexer->current - lexer->start),
                   "unterminated string literal (newline in string)");
      return make_token(lexer, TOKEN_ERROR, lexer->line, start_col);
    } else {
      advance(lexer);
    }
  }

  if (peek(lexer) != quote) {
    report_error(lexer, DIAG_ERROR_UNTERMINATED_STRING, start_col,
                 (size_t)(lexer->current - lexer->start),
                 "unterminated string literal");
    return make_token(lexer, TOKEN_ERROR, lexer->line, start_col);
  }

  advance(lexer);
  return make_token(lexer, type, lexer->line, start_col);
}
/**
 * @brief Tokenize a string literal
 * @param lexer Lexer instance
 * @return String token
 */
csq_token lex_string(csq_lexer *lexer) {
  return scan_quoted_literal(lexer, TOKEN_STRING);
}
/**
 * @brief Tokenize a tag literal (@\"...\")
 * @param lexer Lexer instance
 * @return Tag token
 */
csq_token lex_tag(csq_lexer *lexer) {
  size_t start_col = lexer->column;
  advance(lexer);

  if (peek(lexer) != '"' && peek(lexer) != '\'') {
    report_error(lexer, DIAG_ERROR_INVALID_CHAR, start_col, 1,
                 "tag must be followed by a quoted string");
    return make_token(lexer, TOKEN_ERROR, lexer->line, start_col);
  }

  return scan_quoted_literal(lexer, TOKEN_TAG);
}
/**
 * @brief Tokenize an operator or punctuation
 * @details Handles multi-character operators like ==, +=, //, ->, etc.
 * @param lexer Lexer instance
 * @return Operator token
 */
csq_token lex_operator(csq_lexer *lexer) {
  char c = *lexer->current;
  size_t start_col = lexer->column;
  advance(lexer);
  switch (c) {
  case '+':
    if (*lexer->current == '=') {
      advance(lexer);
      return make_token(lexer, TOKEN_PLUS_ASSIGN, lexer->line, start_col);
    } else if (*lexer->current == '+') {
      advance(lexer);
      return make_token(lexer, TOKEN_INCREMENT, lexer->line, start_col);
    }
    return make_token(lexer, TOKEN_PLUS, lexer->line, start_col);
  case '-':
    if (*lexer->current == '=') {
      advance(lexer);
      return make_token(lexer, TOKEN_MINUS_ASSIGN, lexer->line, start_col);
    } else if (*lexer->current == '-') {
      advance(lexer);
      return make_token(lexer, TOKEN_DECREMENT, lexer->line, start_col);
    } else if (*lexer->current == '>') {
      advance(lexer);
      return make_token(lexer, TOKEN_ARROW, lexer->line, start_col);
    }
    return make_token(lexer, TOKEN_MINUS, lexer->line, start_col);
  case '*':
    if (*lexer->current == '=') {
      advance(lexer);
      return make_token(lexer, TOKEN_STAR_ASSIGN, lexer->line, start_col);
    }
    return make_token(lexer, TOKEN_STAR, lexer->line, start_col);
  case '/':
    if (peek(lexer) == '/') {
      // Skip the comment
      advance(lexer);
      while (peek(lexer) && peek(lexer) != '\n') {
        advance(lexer);
      }

      // Recursively call lexer_next to handle the next token - it will skip any
      // whitespace
      return lexer_next(lexer);
    }
    if (*lexer->current == '=') {
      advance(lexer);
      return make_token(lexer, TOKEN_SLASH_ASSIGN, lexer->line, start_col);
    }
    return make_token(lexer, TOKEN_SLASH, lexer->line, start_col);
  case '%':
    return make_token(lexer, TOKEN_PERCENT, lexer->line, start_col);
  case '^':
    return make_token(lexer, TOKEN_CARET, lexer->line, start_col);
  case '=':
    if (*lexer->current == '=') {
      advance(lexer);
      return make_token(lexer, TOKEN_EQUAL, lexer->line, start_col);
    }
    return make_token(lexer, TOKEN_ASSIGN, lexer->line, start_col);
  case '!':
    if (*lexer->current == '=') {
      advance(lexer);
      return make_token(lexer, TOKEN_NOT_EQUAL, lexer->line, start_col);
    }
    return make_token(lexer, TOKEN_BANG, lexer->line, start_col);
  case '<':
    if (*lexer->current == '=') {
      advance(lexer);
      return make_token(lexer, TOKEN_LESS_EQUAL, lexer->line, start_col);
    }
    return make_token(lexer, TOKEN_LESS, lexer->line, start_col);
  case '>':
    if (*lexer->current == '=') {
      advance(lexer);
      return make_token(lexer, TOKEN_GREATER_EQUAL, lexer->line, start_col);
    }
    return make_token(lexer, TOKEN_GREATER, lexer->line, start_col);
  case '&':
    if (*lexer->current == '&') {
      advance(lexer);
      return make_token(lexer, TOKEN_LOGICAL_AND, lexer->line, start_col);
    }
    return make_token(lexer, TOKEN_AMPERSAND, lexer->line, start_col);
  case '|':
    if (*lexer->current == '|') {
      advance(lexer);
      return make_token(lexer, TOKEN_LOGICAL_OR, lexer->line, start_col);
    }
    return make_token(lexer, TOKEN_PIPE, lexer->line, start_col);
  case '.':
    if (*lexer->current == '.') {
      advance(lexer);
      if (*lexer->current == '.') {
        advance(lexer);
        return make_token(lexer, TOKEN_TRIPLE_DOT, lexer->line, start_col);
      }
      return make_token(lexer, TOKEN_DOUBLE_DOT, lexer->line, start_col);
    }
    return make_token(lexer, TOKEN_DOT, lexer->line, start_col);
  case '(':
    return make_token(lexer, TOKEN_OPEN_PAREN, lexer->line, start_col);
  case ')':
    return make_token(lexer, TOKEN_CLOSE_PAREN, lexer->line, start_col);
  case '{':
    return make_token(lexer, TOKEN_OPEN_BRACE, lexer->line, start_col);
  case '}':
    return make_token(lexer, TOKEN_CLOSE_BRACE, lexer->line, start_col);
  case '[':
    return make_token(lexer, TOKEN_OPEN_BRACKET, lexer->line, start_col);
  case ']':
    return make_token(lexer, TOKEN_CLOSE_BRACKET, lexer->line, start_col);
  case ':':
    return make_token(lexer, TOKEN_COLON, lexer->line, start_col);
  case ';':
    return make_token(lexer, TOKEN_SEMICOLON, lexer->line, start_col);
  case ',':
    return make_token(lexer, TOKEN_COMMA, lexer->line, start_col);
  case '#':
    return make_token(lexer, TOKEN_HASH, lexer->line, start_col);
  default:
    report_error(lexer, DIAG_ERROR_INVALID_CHAR, start_col, 1,
                 "invalid character encountered");
    return make_token(lexer, TOKEN_ERROR, lexer->line, start_col);
  }
}
/**
 * @brief Get next token from lexer
 * @details Skips whitespace and returns the next logical token from the input
 * stream.
 * @param lexer Lexer instance
 * @return Next token
 */
csq_token lexer_next(csq_lexer *lexer) {
  // Always skip whitespace first - unify whitespace handling logic
  while (*lexer->current &&
         (*lexer->current == ' ' || *lexer->current == '\t' ||
          *lexer->current == '\n' || *lexer->current == '\r' ||
          *lexer->current == '\v' || *lexer->current == '\f')) {
    advance(lexer);
  }

  lexer->start = lexer->current;

  if (!*lexer->current) {
    return make_token(lexer, TOKEN_EOF, lexer->line, lexer->column);
  }

  csq_lexstate state = get_lex_state((unsigned char)*lexer->current);
  if (state) {
    return state(lexer);
  }

  size_t start_col = lexer->column;
  advance(lexer);
  report_error(lexer, DIAG_ERROR_UNRECOGNIZED_TOKEN, start_col, 1,
               "unrecognized token");
  return make_token(lexer, TOKEN_ERROR, lexer->line, start_col);
}
/**
 * @brief Print token information to stdout
 * @details Displays token type, value, line number, and column for debugging.
 * @param token Token to print
 */
void lexer_print_token(const csq_token *token) {
  const char *type_str = token_type_to_string(token->type);
  char buffer[256];
  size_t len = token->length;
  if (len > sizeof(buffer) - 1) {
    len = sizeof(buffer) - 1;
  }
  memcpy(buffer, token->start, len);
  buffer[len] = '\0';

  printf("Token type: %s, Value: \"%s\", Line: %zu, Column: %zu\n", type_str,
         buffer, token->line, token->column);
}
