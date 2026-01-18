#ifndef INCLUDE_INCLUDE_PARSER_H_
#define INCLUDE_INCLUDE_PARSER_H_

#include "raven/lexer/lexer.h"
#include "raven/parser/tree.h"
#include <stddef.h>

tree_t *parse(token_list_t *tokenstokens);

/* Literals */
tree_t *parse_literal(token_list_t *tokens, size_t index);
tree_t *parse_literal_array(token_list_t *tokens, size_t index);
tree_t *parse_literal_map(token_list_t *tokens, size_t index);

/* Types */
tree_t *parse_type(token_list_t *tokens, size_t index);

/* Declarations */
tree_t *parse_declaration(token_list_t *tokens, size_t index);
tree_t *parse_decl_variable(token_list_t *tokens, size_t index);
tree_t *parse_decl_function(token_list_t *tokens, size_t index);
tree_t *parse_decl_struct(token_list_t *tokens, size_t index);
tree_t *parse_decl_enum(token_list_t *tokens, size_t index);

/* Statements */
tree_t *parse_statement(token_list_t *tokens, size_t index);
tree_t *parse_stmt_if(token_list_t *tokens, size_t index);

/* Expressions */
tree_t *parse_expr(token_list_t *tokens, size_t index);
tree_t *parse_expr_assignment(token_list_t *tokens, size_t index);
tree_t *parse_expr_logic_or(token_list_t *tokens, size_t index);
tree_t *parse_expr_logic_and(token_list_t *tokens, size_t index);
tree_t *parse_expr_equality(token_list_t *tokens, size_t index);
tree_t *parse_expr_relational(token_list_t *tokens, size_t index);
tree_t *parse_expr_additive(token_list_t *tokens, size_t index);
tree_t *parse_expr_multiplicative(token_list_t *tokens, size_t index);
tree_t *parse_expr_exponent(token_list_t *tokens, size_t index);
tree_t *parse_expr_unary(token_list_t *tokens, size_t index);
tree_t *parse_expr_primary(token_list_t *tokens, size_t index);
tree_t *parse_expr_postfix(token_list_t *tokens, size_t index);
tree_t *parse_expr_range(token_list_t *tokens, size_t index);

#endif // INCLUDE_INCLUDE_PARSER_H_
