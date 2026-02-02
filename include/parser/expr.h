#ifndef PARSER_EXPR_H
#define PARSER_EXPR_H

#include <parser/parser.h>

rvn_node *expr_parse(rvn_parser *parser);
rvn_node *expr_parse_assignment(rvn_parser *parser);
rvn_node *expr_parse_or(rvn_parser *parser);
rvn_node *expr_parse_and(rvn_parser *parser);
rvn_node *expr_parse_equality(rvn_parser *parser);
rvn_node *expr_parse_comparison(rvn_parser *parser);
rvn_node *expr_parse_additive(rvn_parser *parser);
rvn_node *expr_parse_multiplicative(rvn_parser *parser);
rvn_node *expr_parse_unary(rvn_parser *parser);
rvn_node *expr_parse_postfix(rvn_parser *parser);
rvn_node *expr_parse_primary(rvn_parser *parser);

#endif
