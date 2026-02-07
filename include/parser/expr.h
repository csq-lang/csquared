/**
 * @file expr.h
 * @brief Expression parsing functions for the C² parser.
 * @details Provides a suite of recursive descent parsing functions for handling expressions
 * with proper operator precedence and associativity. Each function handles a different
 * precedence level in the expression grammar.
 */

#ifndef PARSER_EXPR_H
#define PARSER_EXPR_H

#include <parser/parser.h>

/**
 * @brief Parses a complete expression.
 * @details Entry point for expression parsing. Handles assignments and all lower-precedence operators.
 * @param parser The parser context.
 * @return Pointer to the parsed expression AST node.
 */
csq_node* expr_parse(csq_parser* parser);

/**
 * @brief Parses assignment expressions.
 * @details Handles assignment operators (=, +=, -=, etc.) with right associativity.
 * @param parser The parser context.
 * @return Pointer to the parsed expression AST node.
 */
csq_node* expr_parse_assignment(csq_parser* parser);

/**
 * @brief Parses logical OR expressions.
 * @details Handles the logical OR operator with short-circuit evaluation semantics.
 * @param parser The parser context.
 * @return Pointer to the parsed expression AST node.
 */
csq_node* expr_parse_or(csq_parser* parser);

/**
 * @brief Parses logical AND expressions.
 * @details Handles the logical AND operator with short-circuit evaluation semantics.
 * @param parser The parser context.
 * @return Pointer to the parsed expression AST node.
 */
csq_node* expr_parse_and(csq_parser* parser);

/**
 * @brief Parses equality expressions.
 * @details Handles equality operators (==, !=).
 * @param parser The parser context.
 * @return Pointer to the parsed expression AST node.
 */
csq_node* expr_parse_equality(csq_parser* parser);

/**
 * @brief Parses bitwise XOR expressions.
 * @details Handles the bitwise XOR operator.
 * @param parser The parser context.
 * @return Pointer to the parsed expression AST node.
 */
csq_node* expr_parse_bitwise_xor(csq_parser* parser);

/**
 * @brief Parses bitwise OR expressions.
 * @details Handles the bitwise OR operator.
 * @param parser The parser context.
 * @return Pointer to the parsed expression AST node.
 */
csq_node* expr_parse_bitwise_or(csq_parser* parser);

/**
 * @brief Parses bitwise AND expressions.
 * @details Handles the bitwise AND operator.
 * @param parser The parser context.
 * @return Pointer to the parsed expression AST node.
 */
csq_node* expr_parse_bitwise_and(csq_parser* parser);

/**
 * @brief Parses comparison expressions.
 * @details Handles comparison operators (<, >, <=, >=).
 * @param parser The parser context.
 * @return Pointer to the parsed expression AST node.
 */
csq_node* expr_parse_comparison(csq_parser* parser);

/**
 * @brief Parses additive expressions.
 * @details Handles addition and subtraction operators.
 * @param parser The parser context.
 * @return Pointer to the parsed expression AST node.
 */
csq_node* expr_parse_additive(csq_parser* parser);

/**
 * @brief Parses multiplicative expressions.
 * @details Handles multiplication, division, and modulo operators.
 * @param parser The parser context.
 * @return Pointer to the parsed expression AST node.
 */
csq_node* expr_parse_multiplicative(csq_parser* parser);

/**
 * @brief Parses unary expressions.
 * @details Handles unary operators (-, !, ~, ++, --, &, *).
 * @param parser The parser context.
 * @return Pointer to the parsed expression AST node.
 */
csq_node* expr_parse_unary(csq_parser* parser);

/**
 * @brief Parses postfix expressions.
 * @details Handles postfix operators (++, --), function calls, array indexing, and member access.
 * @param parser The parser context.
 * @return Pointer to the parsed expression AST node.
 */
csq_node* expr_parse_postfix(csq_parser* parser);

/**
 * @brief Parses primary expressions.
 * @details Handles literals, identifiers, parenthesized expressions, and other atomic expressions.
 * @param parser The parser context.
 * @return Pointer to the parsed expression AST node.
 */
csq_node* expr_parse_primary(csq_parser* parser);

#endif