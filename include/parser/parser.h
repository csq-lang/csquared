#ifndef PARSER_PARSER_H
#define PARSER_PARSER_H

#include <csquare/lexer.h>
#include <csquare/token.h>
#include <parser/ast.h>
#include <core/diag.h>
#include <parser/func.h>
#include <parser/var.h>
#include <parser/stmt.h>
#include <parser/struct.h>

typedef struct csq_parser {
    csq_lexer* lexer;
    DiagReporter* diag;
    csq_token current;
    csq_token previous;
    bool had_error;
    bool panic_mode;
    ast_context* ast;
} csq_parser;

csq_parser* parser_create(csq_lexer* lexer, DiagReporter* diag);
void parser_free(csq_parser* parser);

ast_context* parser_parse(csq_parser* parser);

csq_node* parser_parse_expression(csq_parser* parser);
csq_node* parser_parse_statement(csq_parser* parser);
csq_node* parser_parse_declaration(csq_parser* parser);
csq_node* parser_parse_type(csq_parser* parser);

csq_node* parse_identifier_node(csq_parser* parser);

bool parser_match(csq_parser* parser, csq_tktype type);
bool parser_check(csq_parser* parser, csq_tktype type);
void parser_advance(csq_parser* parser);
void parser_consume(csq_parser* parser, csq_tktype type, const char* message);

void parser_error(csq_parser* parser, const char* message);
void parser_error_at(csq_parser* parser, csq_token* token, const char* message);
void parser_error_at_location(csq_parser* parser, DiagErrorType type,
                              size_t line, size_t column, size_t length,
                              const char* message);
void parser_synchronize(csq_parser* parser);

#endif