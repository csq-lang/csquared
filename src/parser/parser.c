#include <parser/expr.h>
#include <parser/parser.h>
#include <stdlib.h>
#include <string.h>

rvn_parser *parser_create(rvn_lexer *lexer, DiagReporter *diag) {
  rvn_parser *parser = calloc(1, sizeof(rvn_parser));
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

void parser_free(rvn_parser *parser) {
  if (!parser)
    return;
  if (parser->ast) {
    ast_context_free(parser->ast);
  }
  free(parser);
}

void parser_advance(rvn_parser *parser) {
  parser->previous = parser->current;

  for (;;) {
    parser->current = lexer_next(parser->lexer);
    if (parser->current.type != TOKEN_ERROR)
      break;

    parser_error_at(parser, &parser->current, "Invalid token");
  }
}

bool parser_check(rvn_parser *parser, TokenType type) {
  return parser->current.type == type;
}

bool parser_match(rvn_parser *parser, TokenType type) {
  if (!parser_check(parser, type))
    return false;
  parser_advance(parser);
  return true;
}

void parser_consume(rvn_parser *parser, TokenType type, const char *message) {
  if (parser->current.type == type) {
    parser_advance(parser);
    return;
  }
  parser_error(parser, message);
}

void parser_error_at(rvn_parser *parser, Token *token, const char *message) {
  if (parser->panic_mode)
    return;
  parser->panic_mode = true;
  parser->had_error = true;

  diag_report_error(parser->diag, DIAG_ERROR_UNRECOGNIZED_TOKEN,
                    parser->lexer->path, token->line, token->column,
                    token->length, NULL, message);
}

void parser_error(rvn_parser *parser, const char *message) {
  parser_error_at(parser, &parser->current, message);
}

void parser_synchronize(rvn_parser *parser) {
  parser->panic_mode = false;

  while (parser->current.type != TOKEN_EOF) {
    if (parser->previous.type == TOKEN_SEMICOLON)
      return;

    switch (parser->current.type) {
    case TOKEN_KEYWORD_FUNCTION:
    case TOKEN_KEYWORD_FUNC:
    case TOKEN_KEYWORD_IF:
    case TOKEN_KEYWORD_WHILE:
    case TOKEN_KEYWORD_FOR:
    case TOKEN_KEYWORD_RETURN:
    case TOKEN_KEYWORD_STRUCT:
    case TOKEN_KEYWORD_ENUM:
    case TOKEN_KEYWORD_IMPORT:
      return;
    default:
      break;
    }

    parser_advance(parser);
  }
}

rvn_node *parser_parse_expression(rvn_parser *parser) {
  return expr_parse(parser);
}

rvn_node *parse_identifier_node(rvn_parser *parser) {
  Token token = parser->previous;
  rvn_node *node = node_create(NODE_IDENTIFIER, token.line, token.column);
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

rvn_node *parse_block(rvn_parser *parser) {
  rvn_node *block =
      node_create(NODE_BLOCK, parser->previous.line, parser->previous.column);
  if (!block)
    return NULL;

  node_list_init(&block->data.block.statements);

  while (!parser_check(parser, TOKEN_CLOSE_BRACE) &&
         !parser_check(parser, TOKEN_EOF)) {
    rvn_node *stmt = parser_parse_statement(parser);
    if (stmt)
      node_list_add(&block->data.block.statements, stmt);
  }

  parser_consume(parser, TOKEN_CLOSE_BRACE, "Expected '}' after block");
  return block;
}

rvn_node *parse_if_statement(rvn_parser *parser) {
  rvn_node *node =
      node_create(NODE_IF, parser->previous.line, parser->previous.column);
  if (!node)
    return NULL;

  node->data.if_stmt.condition = parser_parse_expression(parser);

  parser_consume(parser, TOKEN_OPEN_BRACE, "Expected '{' after if condition");
  node->data.if_stmt.then_branch = parse_block(parser);

  if (parser_match(parser, TOKEN_KEYWORD_ELSE)) {
    if (parser_match(parser, TOKEN_KEYWORD_IF)) {
      node->data.if_stmt.else_branch = parse_if_statement(parser);
    } else {
      parser_consume(parser, TOKEN_OPEN_BRACE, "Expected '{' after else");
      node->data.if_stmt.else_branch = parse_block(parser);
    }
  }

  return node;
}

rvn_node *parse_while_statement(rvn_parser *parser) {
  rvn_node *node =
      node_create(NODE_WHILE, parser->previous.line, parser->previous.column);
  if (!node)
    return NULL;

  node->data.while_stmt.condition = parser_parse_expression(parser);

  parser_consume(parser, TOKEN_OPEN_BRACE,
                 "Expected '{' after while condition");
  node->data.while_stmt.body = parse_block(parser);

  return node;
}

rvn_node *parse_for_statement(rvn_parser *parser) {
  rvn_node *node =
      node_create(NODE_FOR, parser->previous.line, parser->previous.column);
  if (!node)
    return NULL;

  parser_consume(parser, TOKEN_IDENTIFIER, "Expected identifier after for");
  node->data.for_stmt.var = parse_identifier_node(parser);

  parser_consume(parser, TOKEN_KEYWORD_IN, "Expected 'in' after for variable");
  node->data.for_stmt.iterable = parser_parse_expression(parser);

  parser_consume(parser, TOKEN_OPEN_BRACE, "Expected '{' after for iterable");
  node->data.for_stmt.body = parse_block(parser);

  return node;
}

rvn_node *parse_return_statement(rvn_parser *parser) {
  rvn_node *node =
      node_create(NODE_RETURN, parser->previous.line, parser->previous.column);
  if (!node)
    return NULL;

  if (!parser_check(parser, TOKEN_SEMICOLON) &&
      !parser_check(parser, TOKEN_CLOSE_BRACE))
    node->data.return_stmt.value = parser_parse_expression(parser);

  return node;
}

rvn_node *parse_var_declaration(rvn_parser *parser, bool is_const) {

  Token type_token = parser->current;
  parser_advance(parser);

  Token name_token = parser->current;
  parser_consume(parser, TOKEN_IDENTIFIER, "Expected variable name");

  rvn_node *node;
  if (is_const)
    node = node_create(NODE_CONST_DECL, name_token.line, name_token.column);
  else
    node = node_create(NODE_VAR_DECL, name_token.line, name_token.column);

  if (!node)
    return NULL;

  parser->previous = name_token;
  node->data.var_decl.name = parse_identifier_node(parser);
  node->data.var_decl.is_mutable = !is_const;

  rvn_node *type_spec =
      node_create(NODE_TYPE_SPEC, type_token.line, type_token.column);
  if (type_spec) {
    parser->previous = type_token;
    type_spec->data.type_spec.base = parse_identifier_node(parser);
    node_list_init(&type_spec->data.type_spec.args);
    node->data.var_decl.type_spec = type_spec;
  }

  if (parser_match(parser, TOKEN_ASSIGN))
    node->data.var_decl.init = parser_parse_expression(parser);
  else if (is_const)
    parser_error(parser, "Constants must be initialized");

  return node;
}

rvn_node *parse_function_declaration(rvn_parser *parser) {
  Token name_token = parser->current;
  parser_consume(parser, TOKEN_IDENTIFIER, "Expected function name");

  rvn_node *node =
      node_create(NODE_FUNCTION_DECL, name_token.line, name_token.column);
  if (!node)
    return NULL;

  node->data.function_decl.name = parse_identifier_node(parser);
  node_list_init(&node->data.function_decl.params);
  node_list_init(&node->data.function_decl.generics);

  parser_consume(parser, TOKEN_OPEN_PAREN, "Expected '(' after function name");

  if (!parser_check(parser, TOKEN_CLOSE_PAREN)) {
    do {
      Token param_token = parser->current;
      parser_consume(parser, TOKEN_IDENTIFIER, "Expected parameter name");

      rvn_node *param =
          node_create(NODE_PARAM, param_token.line, param_token.column);
      if (!param)
        continue;

      param->data.param.name = parse_identifier_node(parser);

      if (parser_match(parser, TOKEN_COLON))
        param->data.param.type_spec = parse_type_spec(parser);
      if (parser_match(parser, TOKEN_ASSIGN))
        param->data.param.default_val = parser_parse_expression(parser);

      node_list_add(&node->data.function_decl.params, param);
    } while (parser_match(parser, TOKEN_COMMA));
  }

  parser_consume(parser, TOKEN_CLOSE_PAREN, "Expected ')' after parameters");

  if (parser_match(parser, TOKEN_COLON)) {
    node->data.function_decl.return_type = parse_type_spec(parser);
  }

  parser_consume(parser, TOKEN_OPEN_BRACE, "Expected '{' before function body");
  node->data.function_decl.body = parse_block(parser);

  return node;
}

rvn_node *parse_func_declaration(rvn_parser *parser) {
  Token name_token = parser->current;
  parser_consume(parser, TOKEN_IDENTIFIER, "Expected function name");

  rvn_node *node =
      node_create(NODE_FUNCTION_DECL, name_token.line, name_token.column);
  if (!node)
    return NULL;

  node->data.function_decl.name = parse_identifier_node(parser);
  node_list_init(&node->data.function_decl.params);
  node_list_init(&node->data.function_decl.generics);

  parser_consume(parser, TOKEN_OPEN_PAREN, "Expected '(' after function name");

  if (!parser_check(parser, TOKEN_CLOSE_PAREN)) {
    do {
      rvn_node *param =
          node_create(NODE_PARAM, parser->current.line, parser->current.column);
      if (!param)
        continue;

      param->data.param.type_spec = parse_type_spec(parser);

      parser_consume(parser, TOKEN_IDENTIFIER, "Expected parameter name");
      param->data.param.name = parse_identifier_node(parser);

      if (parser_match(parser, TOKEN_ASSIGN)) {
        param->data.param.default_val = parser_parse_expression(parser);
      }

      node_list_add(&node->data.function_decl.params, param);
    } while (parser_match(parser, TOKEN_COMMA));
  }

  parser_consume(parser, TOKEN_CLOSE_PAREN, "Expected ')' after parameters");

  if (parser_match(parser, TOKEN_ARROW)) {
    node->data.function_decl.return_type = parse_type_spec(parser);
  }

  parser_consume(parser, TOKEN_OPEN_BRACE, "Expected '{' before function body");
  node->data.function_decl.body = parse_block(parser);

  return node;
}

rvn_node *parse_type_spec(rvn_parser *parser) {
  rvn_node *node =
      node_create(NODE_TYPE_SPEC, parser->current.line, parser->current.column);
  if (!node)
    return NULL;

  if (parser_match(parser, TOKEN_OPEN_BRACKET)) {
    rvn_node *elem_type = parse_type_spec(parser);
    parser_consume(parser, TOKEN_SEMICOLON, "Expected ';' in array type");
    parser_consume(parser, TOKEN_NUMBER, "Expected array size");
    Token size_token = parser->previous;
    parser_consume(parser, TOKEN_CLOSE_BRACKET,
                   "Expected ']' after array type");

    node->data.type_spec.base = elem_type;
    node_list_init(&node->data.type_spec.args);

    rvn_node *size_node =
        node_create(NODE_LITERAL_INT, size_token.line, size_token.column);
    char *end;
    size_node->data.literal_int.value = strtoll(size_token.start, &end, 0);
    node_list_add(&node->data.type_spec.args, size_node);

    return node;
  }

  if (parser_match(parser, TOKEN_IDENTIFIER) ||
      parser_match(parser, TOKEN_KEYWORD_INT) ||
      parser_match(parser, TOKEN_KEYWORD_STRING) ||
      parser_match(parser, TOKEN_KEYWORD_FLOAT)) {
    node->data.type_spec.base = parse_identifier_node(parser);
  } else {
    parser_error(parser, "Expected type name");
    return node;
  }

  node_list_init(&node->data.type_spec.args);

  return node;
}

rvn_node *parser_parse_statement(rvn_parser *parser) {

  if (parser_check(parser, TOKEN_KEYWORD_INT) ||
      parser_check(parser, TOKEN_KEYWORD_STRING) ||
      parser_check(parser, TOKEN_KEYWORD_FLOAT)) {
    return parse_var_declaration(parser, false);
  }

  if (parser_match(parser, TOKEN_KEYWORD_IF)) {
    rvn_node *node = parse_if_statement(parser);
    return node;
  }

  if (parser_match(parser, TOKEN_KEYWORD_WHILE)) {
    rvn_node *node = parse_while_statement(parser);
    return node;
  }

  if (parser_match(parser, TOKEN_KEYWORD_FOR)) {
    rvn_node *node = parse_for_statement(parser);
    return node;
  }

  if (parser_match(parser, TOKEN_KEYWORD_RETURN)) {
    rvn_node *node = parse_return_statement(parser);
    return node;
  }

  if (parser_match(parser, TOKEN_OPEN_BRACE))
    return parse_block(parser);

  return parse_expression_statement(parser);
}

rvn_node *parse_expression_statement(rvn_parser *parser) {
  rvn_node *expr = parser_parse_expression(parser);
  if (!expr)
    return NULL;

  rvn_node *node = node_create(NODE_EXPR_STMT, expr->line, expr->column);
  if (!node)
    return expr;

  node->data.expr_stmt.expr = expr;
  return node;
}

rvn_node *parser_parse_declaration(rvn_parser *parser) {
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

ast_context *parser_parse(rvn_parser *parser) {
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
    rvn_node *decl = parser_parse_declaration(parser);
    if (decl)
      node_list_add(&parser->ast->root->data.program.statements, decl);
    else {
      rvn_node *stmt = parser_parse_statement(parser);
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
