#ifndef PARSER_PARSER_H
#define PARSER_PARSER_H

#include <core/diag.h>
#include <parser/ast.h>
#include <raven/lexer.h>
#include <raven/token.h>

typedef struct rvn_parser {
  rvn_lexer *lexer;
  DiagReporter *diag;
  Token current;
  Token previous;
  bool had_error;
  bool panic_mode;
  ast_context *ast;
} rvn_parser;

rvn_parser *parser_create(rvn_lexer *lexer, DiagReporter *diag);
void parser_free(rvn_parser *parser);

ast_context *parser_parse(rvn_parser *parser);

rvn_node *parser_parse_expression(rvn_parser *parser);
rvn_node *parser_parse_statement(rvn_parser *parser);
rvn_node *parser_parse_declaration(rvn_parser *parser);
rvn_node *parser_parse_type(rvn_parser *parser);

rvn_node *parse_expression_statement(rvn_parser *parser);
rvn_node *parse_block(rvn_parser *parser);
rvn_node *parse_declaration(rvn_parser *parser);
rvn_node *parse_type_spec(rvn_parser *parser);
rvn_node *parse_var_declaration(rvn_parser *parser, bool is_const);
rvn_node *parse_function_declaration(rvn_parser *parser);
rvn_node *parse_func_declaration(rvn_parser *parser);
rvn_node *parse_if_statement(rvn_parser *parser);
rvn_node *parse_while_statement(rvn_parser *parser);
rvn_node *parse_for_statement(rvn_parser *parser);
rvn_node *parse_return_statement(rvn_parser *parser);
rvn_node *parse_identifier_node(rvn_parser *parser);

bool parser_match(rvn_parser *parser, TokenType type);
bool parser_check(rvn_parser *parser, TokenType type);
void parser_advance(rvn_parser *parser);
void parser_consume(rvn_parser *parser, TokenType type, const char *message);

void parser_error(rvn_parser *parser, const char *message);
void parser_error_at(rvn_parser *parser, Token *token, const char *message);
void parser_synchronize(rvn_parser *parser);

#endif
