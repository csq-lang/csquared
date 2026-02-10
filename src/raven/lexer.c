#include <ctype.h>
#include <raven/lexer.h>
#include <raven/logger.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static rvn_lexstate state_table[256];

static void init_state_table(void) {
  static int initialized = 0;
  if (initialized)
    return;

  for (int i = 0; i < 256; i++) {
    if (isspace((unsigned char)i))
      state_table[i] = lex_whitespace;
    else if (isalpha((unsigned char)i) || i == '_')
      state_table[i] = lex_identifier;
    else if (isdigit((unsigned char)i))
      state_table[i] = lex_number;
    else if (i == '"' || i == '\'')
      state_table[i] = lex_string;
    else if (i == '@')
      state_table[i] = lex_tag;
    else if (i == '\0')
      state_table[i] = NULL;
    else
      state_table[i] = lex_operator;
  }

  initialized = 1;
}

rvn_lexer *lexer_create(const rvn_source *source, DiagReporter *diag) {
  init_state_table();

  rvn_lexer *lexer = malloc(sizeof(rvn_lexer));
  if (!lexer) {
    RVN_FATAL("Not enough memory to create lexer");
    return NULL;
  }

  lexer->buffer = source->buffer;
  lexer->start = source->buffer;
  lexer->current = source->buffer;
  lexer->line = 1;
  lexer->column = 1;
  lexer->path = source->path;
  lexer->diag = diag;

  return lexer;
}

void lexer_free(rvn_lexer *lexer) { free(lexer); }

static void advance(rvn_lexer *lexer) {
  if (*lexer->current == '\n') {
    lexer->line++;
    lexer->column = 1;
  } else
    lexer->column++;
  lexer->current++;
}

static char peek(rvn_lexer *lexer) { return *lexer->current; }

static char peek_next(rvn_lexer *lexer) {
  if (*lexer->current == '\0')
    return '\0';
  return *(lexer->current + 1);
}

static Token make_token(rvn_lexer *lexer, TokenType type, size_t line,
                        size_t column) {
  Token token;
  token.type = type;
  token.start = lexer->start;
  token.length = (size_t)(lexer->current - lexer->start);
  token.line = line;
  token.column = column;
  return token;
}

static void report_error(rvn_lexer *lexer, DiagErrorType type, size_t start_col,
                         size_t length, const char *message) {
  if (lexer->diag) {
    diag_report(lexer->diag, type, lexer->path, lexer->line, start_col, length,
                message);
  }
}

Token lex_whitespace(rvn_lexer *lexer) {
  while (isspace((unsigned char)*lexer->current)) {
    advance(lexer);
  }
  return lexer_next(lexer);
}

static TokenType check_keyword(const char *start, size_t length) {
  switch (start[0]) {
  case 'a':
    if (length == 3 && start[1] == 'n' && start[2] == 'd')
      return TOKEN_KEYWORD_AND;
    break;
  case 'c':
    if (length == 5) {
      if (start[1] == 'a' && start[2] == 't' && start[3] == 'c' &&
          start[4] == 'h')
        return TOKEN_KEYWORD_CATCH;
    }
    if (length == 4 && start[1] == 'a' && start[2] == 's' && start[3] == 'e')
      return TOKEN_KEYWORD_CASE;
    break;
  case 'd':
    if (length == 5 && start[1] == 'e' && start[2] == 'f' && start[3] == 'e' &&
        start[4] == 'r')
      return TOKEN_KEYWORD_DEFER;
    if (length == 7 && start[1] == 'e' && start[2] == 'f' && start[3] == 'a' &&
        start[4] == 'u' && start[5] == 'l' && start[6] == 't')
      return TOKEN_KEYWORD_DEFAULT;
    break;
  case 'e':
    if (length == 4) {
      if (start[1] == 'l' && start[2] == 's' && start[3] == 'e')
        return TOKEN_KEYWORD_ELSE;
      if (start[1] == 'n' && start[2] == 'u' && start[3] == 'm')
        return TOKEN_KEYWORD_ENUM;
    }
    break;
  case 'f':
    if (length == 5 && start[1] == 'a' && start[2] == 'l' && start[3] == 's' &&
        start[4] == 'e')
      return TOKEN_KEYWORD_FALSE;
    if (length == 3 && start[1] == 'o' && start[2] == 'r')
      return TOKEN_KEYWORD_FOR;
    if (length == 4 && start[1] == 'u' && start[2] == 'n' && start[3] == 'c')
      return TOKEN_KEYWORD_FUNC;
    if (length == 8 && start[1] == 'u' && start[2] == 'n' && start[3] == 'c' &&
        start[4] == 't' && start[5] == 'i' && start[6] == 'o' &&
        start[7] == 'n')
      return TOKEN_KEYWORD_FUNCTION;
    if (length == 5 && start[1] == 'l' && start[2] == 'o' && start[3] == 'a' &&
        start[4] == 't')
      return TOKEN_KEYWORD_FLOAT;
    break;
  case 'i':
    if (length == 2 && start[1] == 'f')
      return TOKEN_KEYWORD_IF;
    if (length == 2 && start[1] == 'n')
      return TOKEN_KEYWORD_IN;
    if (length == 6 && start[1] == 'm' && start[2] == 'p' && start[3] == 'o' &&
        start[4] == 'r' && start[5] == 't')
      return TOKEN_KEYWORD_IMPORT;
    if (length == 3 && start[1] == 'n' && start[2] == 't')
      return TOKEN_KEYWORD_INT;
    break;

  case 'n':
    if (length == 3 && start[1] == 'e' && start[2] == 'w')
      return TOKEN_KEYWORD_NEW;
    break;
  case 'o':
    if (length == 2 && start[1] == 'r')
      return TOKEN_KEYWORD_OR;
    break;
  case 'p':
    if (length == 7 && start[1] == 'r' && start[2] == 'i' && start[3] == 'v' &&
        start[4] == 'a' && start[5] == 't' && start[6] == 'e')
      return TOKEN_KEYWORD_PRIVATE;
    break;
  case 'r':
    if (length == 6 && start[1] == 'e' && start[2] == 'p' && start[3] == 'e' &&
        start[4] == 'a' && start[5] == 't')
      return TOKEN_KEYWORD_REPEAT;
    if (length == 6 && start[1] == 'e' && start[2] == 't' && start[3] == 'u' &&
        start[4] == 'r' && start[5] == 'n')
      return TOKEN_KEYWORD_RETURN;
    break;
  case 's':
    if (length == 4) {
      if (start[1] == 'e' && start[2] == 'l' && start[3] == 'f')
        return TOKEN_KEYWORD_SELF;
      if (start[1] == 'p' && start[2] == 'a' && start[3] == 'w')
        return TOKEN_KEYWORD_SPAWN;
    }
    if (length == 6 && start[1] == 't' && start[2] == 'r' && start[3] == 'u' &&
        start[4] == 'c' && start[5] == 't')
      return TOKEN_KEYWORD_STRUCT;
    if (length == 6 && start[1] == 'w' && start[2] == 'i' && start[3] == 't' &&
        start[4] == 'c' && start[5] == 'h')
      return TOKEN_KEYWORD_SWITCH;
    if (length == 6 && start[1] == 't' && start[2] == 'r' && start[3] == 'i' &&
        start[4] == 'n' && start[5] == 'g')
      return TOKEN_KEYWORD_STRING;
    break;
  case 't':
    if (length == 4 && start[1] == 'r' && start[2] == 'u' && start[3] == 'e')
      return TOKEN_KEYWORD_TRUE;
    if (length == 3 && start[1] == 'r' && start[2] == 'y')
      return TOKEN_KEYWORD_TRY;
    if (length == 5 && start[1] == 'h' && start[2] == 'r' && start[3] == 'o' &&
        start[4] == 'w')
      return TOKEN_KEYWORD_THROW;
    break;
  case 'u':
    if (length == 5 && start[1] == 'n' && start[2] == 't' && start[3] == 'i' &&
        start[4] == 'l')
      return TOKEN_KEYWORD_UNTIL;
    break;
  case 'w':
    if (length == 5 && start[1] == 'h' && start[2] == 'i' && start[3] == 'l' &&
        start[4] == 'e')
      return TOKEN_KEYWORD_WHILE;
    break;
  }
  return TOKEN_IDENTIFIER;
}

Token lex_identifier(rvn_lexer *lexer) {
  size_t start_col = lexer->column;

  while (isalnum((unsigned char)*lexer->current) || *lexer->current == '_') {
    advance(lexer);
  }

  size_t length = (size_t)(lexer->current - lexer->start);
  TokenType type = check_keyword(lexer->start, length);

  if (type == TOKEN_IDENTIFIER) {
    if (length > 0 && isdigit((unsigned char)lexer->start[0])) {
      report_error(lexer, DIAG_ERROR_INVALID_IDENTIFIER, start_col, length,
                   "identifier cannot start with a digit");
      return make_token(lexer, TOKEN_ERROR, lexer->line, start_col);
    }
  }

  return make_token(lexer, type, lexer->line, start_col);
}

static bool is_valid_base_prefix(char c1, char c2) {
  return (c1 == '0' && (c2 == 'x' || c2 == 'X' || c2 == 'b' || c2 == 'B' ||
                        c2 == 'o' || c2 == 'O'));
}

static bool is_digit_in_base(char c, int base) {
  if (base == 2)
    return c == '0' || c == '1';
  if (base == 8)
    return c >= '0' && c <= '7';
  if (base == 10)
    return isdigit((unsigned char)c);
  if (base == 16)
    return isdigit((unsigned char)c) || (c >= 'a' && c <= 'f') ||
           (c >= 'A' && c <= 'F');
  return false;
}

Token lex_number(rvn_lexer *lexer) {
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

static bool is_valid_escape(char c) {
  return c == 'n' || c == 't' || c == 'r' || c == '\\' || c == '"' ||
         c == '\'' || c == '0' || c == 'x';
}

static Token scan_quoted_literal(rvn_lexer *lexer, TokenType type) {
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

Token lex_string(rvn_lexer *lexer) {
  return scan_quoted_literal(lexer, TOKEN_STRING);
}

Token lex_tag(rvn_lexer *lexer) {
  size_t start_col = lexer->column;
  advance(lexer);

  if (peek(lexer) != '"' && peek(lexer) != '\'') {
    report_error(lexer, DIAG_ERROR_INVALID_CHAR, start_col, 1,
                 "tag must be followed by a quoted string");
    return make_token(lexer, TOKEN_ERROR, lexer->line, start_col);
  }

  return scan_quoted_literal(lexer, TOKEN_TAG);
}

Token lex_operator(rvn_lexer *lexer) {
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
      advance(lexer);
      while (peek(lexer) && peek(lexer) != '\n') {
        advance(lexer);
      }
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
    return make_token(lexer, TOKEN_OPERATOR, lexer->line, start_col);
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
    return make_token(lexer, TOKEN_OPERATOR, lexer->line, start_col);
  case '|':
    if (*lexer->current == '|') {
      advance(lexer);
      return make_token(lexer, TOKEN_LOGICAL_OR, lexer->line, start_col);
    }
    return make_token(lexer, TOKEN_OPERATOR, lexer->line, start_col);
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

Token lexer_next(rvn_lexer *lexer) {
  lexer->start = lexer->current;

  if (!*lexer->current) {
    return make_token(lexer, TOKEN_EOF, lexer->line, lexer->column);
  }

  rvn_lexstate state = state_table[(unsigned char)*lexer->current];
  if (state) {
    return state(lexer);
  }

  size_t start_col = lexer->column;
  advance(lexer);
  report_error(lexer, DIAG_ERROR_UNRECOGNIZED_TOKEN, start_col, 1,
               "unrecognized token");
  return make_token(lexer, TOKEN_ERROR, lexer->line, start_col);
}

void lexer_print_token(const Token *token) {
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
