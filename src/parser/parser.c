/**
 * @file parser.c
 * @brief Main parser implementation
 * @details Implements the main parser logic including token consumption,
 * error handling, panic mode recovery, and top-level statement parsing.
 * Orchestrates the various parsing functions for different language constructs.
 */

#include <parser/expr.h>
#include <parser/func.h>
#include <parser/parser.h>
#include <parser/stmt.h>
#include <parser/struct.h>
#include <parser/var.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief Create a new parser instance
 * @param lexer Lexer to tokenize input
 * @param diag Diagnostic reporter for errors
 * @return Allocated parser, or NULL on failure
 */
csq_parser *parser_create(csq_lexer *lexer, DiagReporter *diag) {
  csq_parser *parser = calloc(1, sizeof(csq_parser));
  if (!parser)
    return NULL;

  parser->lexer = lexer;
  parser->diag = diag;
  parser->had_error = false;
  parser->panic_mode = false;
  parser->ast = NULL;

  parser_advance(parser);

  return parser;
}

/**
 * @brief Free parser resources
 * @param parser Parser to deallocate
 */
void parser_free(csq_parser *parser) {
  if (!parser)
    return;
  if (parser->ast) {
    ast_context_free(parser->ast);
  }
  free(parser);
}

/**
 * @brief Advance to next token, skipping errors
 * @param parser Parser instance
 */
void parser_advance(csq_parser *parser) {
  parser->previous = parser->current;

  for (;;) {
    parser->current = lexer_next(parser->lexer);
    if (parser->current.type != TOKEN_ERROR)
      break;

    parser_error_at(parser, &parser->current, "Invalid token");
  }
}

/**
 * @brief Check current token type without consuming
 * @param parser Parser instance
 * @param type Token type to check for
 * @return True if current token matches type
 */
bool parser_check(csq_parser *parser, csq_tktype type) {
  return parser->current.type == type;
}

/**
 * @brief Consume token if it matches type
 * @param parser Parser instance
 * @param type Token type to match
 * @return True if token matched and was consumed
 */
bool parser_match(csq_parser *parser, csq_tktype type) {
  if (parser->current.type != type)
    return false;
  parser_advance(parser);
  return true;
}

/**
 * @brief Consume expected token or report error
 * @param parser Parser instance
 * @param type Expected token type
 * @param message Error message if not found
 */
void parser_consume(csq_parser *parser, csq_tktype type, const char *message) {
  if (parser->current.type == type) {
    parser_advance(parser);
    return;
  }
  parser_error(parser, message);
}

void parser_error_at(csq_parser *parser, csq_token *token,
                     const char *message) {
  if (parser->panic_mode)
    return;
  parser->panic_mode = true;
  parser->had_error = true;

  diag_report_error(parser->diag, DIAG_ERROR_UNRECOGNIZED_TOKEN,
                    parser->lexer->path, token->line, token->column,
                    token->length, NULL, message);
}

void parser_error_at_location(csq_parser *parser, DiagErrorType type,
                              size_t line, size_t column, size_t length,
                              const char *message) {
  if (parser->panic_mode)
    return;
  parser->panic_mode = true;
  parser->had_error = true;

  diag_report_error(parser->diag, type, parser->lexer->path, line, column,
                    length, NULL, message);
}

void parser_error(csq_parser *parser, const char *message) {
  parser_error_at(parser, &parser->current, message);
}

void parser_synchronize(csq_parser *parser) {
  parser->panic_mode = false;

  while (parser->current.type != TOKEN_EOF) {
    if (parser->previous.type == TOKEN_SEMICOLON)
      return;

    switch (parser->current.type) {
    case TOKEN_KEYWORD_FUNCTION:
    case TOKEN_KEYWORD_FUNC:
    case TOKEN_KEYWORD_IF:
    case TOKEN_KEYWORD_ELSE:
    case TOKEN_KEYWORD_WHILE:
    case TOKEN_KEYWORD_FOR:
    case TOKEN_KEYWORD_RETURN:
    case TOKEN_KEYWORD_STRUCT:
    case TOKEN_KEYWORD_ENUM:
    case TOKEN_KEYWORD_IMPORT:
    case TOKEN_OPEN_BRACE:
    case TOKEN_CLOSE_BRACE:
      return;
    default:
      break;
    }

    parser_advance(parser);
  }
}

csq_node *parser_parse_expression(csq_parser *parser) {
  return expr_parse(parser);
}

csq_node *parse_identifier_node(csq_parser *parser) {
  csq_token token = parser->previous;
  csq_node *node = node_create(NODE_IDENTIFIER, token.line, token.column);
  if (!node)
    return NULL;

  char *name = malloc(token.length + 1);
  if (!name) {
    node_free(node);
    return NULL;
  }
  memcpy(name, token.start, token.length);
  name[token.length] = '\0';

  node->data.identifier.name = name;
  node->data.identifier.len = token.length;
  return node;
}

csq_node *parser_parse_statement(csq_parser *parser) {

  if (parser_check(parser, TOKEN_KEYWORD_BOOL) ||
      parser_check(parser, TOKEN_KEYWORD_INT) ||
      parser_check(parser, TOKEN_KEYWORD_STRING) ||
      parser_check(parser, TOKEN_KEYWORD_FLOAT) ||
      parser_check(parser, TOKEN_OPEN_BRACKET)) {
    return parse_var_declaration(parser, false);
  }

  if (parser_match(parser, TOKEN_KEYWORD_IF)) {
    csq_node *node = parse_if_statement(parser);
    return node;
  }

  if (parser_match(parser, TOKEN_KEYWORD_WHILE)) {
    csq_node *node = parse_while_statement(parser);
    return node;
  }

  if (parser_match(parser, TOKEN_KEYWORD_RETURN)) {
    csq_node *node = parse_return_statement(parser);
    return node;
  }

  if (parser_match(parser, TOKEN_OPEN_BRACE))
    return parse_block(parser);

  return parse_expression_statement(parser);
}

csq_node *parser_parse_declaration(csq_parser *parser) {
  if (parser_match(parser, TOKEN_KEYWORD_FUNCTION))
    return parse_function_declaration(parser);

  if (parser_match(parser, TOKEN_KEYWORD_FUNC))
    return parse_func_declaration(parser);

  if (parser_match(parser, TOKEN_KEYWORD_PRIVATE)) {
    if (parser_check(parser, TOKEN_IDENTIFIER))
      return parse_var_declaration(parser, false);
  }

  if (parser_check(parser, TOKEN_KEYWORD_INT) ||
      parser_check(parser, TOKEN_KEYWORD_STRING) ||
      parser_check(parser, TOKEN_KEYWORD_FLOAT)) {
    return parse_var_declaration(parser, false);
  }

  return NULL;
}

ast_context *parser_parse(csq_parser *parser) {
  parser->ast = ast_context_create();
  if (!parser->ast)
    return NULL;

  parser->ast->root = ast_create_program();
  if (!parser->ast->root) {
    ast_context_free(parser->ast);
    parser->ast = NULL;
    return NULL;
  }

  while (!parser_check(parser, TOKEN_EOF)) {
    csq_node *decl = parser_parse_declaration(parser);
    if (decl)
      node_list_add(&parser->ast->root->data.program.statements, decl);
    else {
      csq_node *stmt = parser_parse_statement(parser);
      if (stmt)
        node_list_add(&parser->ast->root->data.program.statements, stmt);
      else
        parser_advance(parser);
    }

    if (parser->panic_mode)
      parser_synchronize(parser);
  }

  parser->ast->has_errors = parser->had_error;
  return parser->ast;
}
