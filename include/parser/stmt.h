/**
 * @file stmt.h
 * @brief Statement parsing functions for the C² parser.
 * @details Provides parsing functions for all statement types including
 * control flow, loops, and expression statements.
 */

#ifndef PARSER_STMT_H
#define PARSER_STMT_H

#include <parser/node.h>

struct csq_parser;

/**
 * @brief Parses a block of statements.
 * @details Parses a sequence of statements enclosed in braces.
 * @param parser The parser context.
 * @return Pointer to a block node containing the statements.
 */
csq_node* parse_block(struct csq_parser* parser);

/**
 * @brief Parses an if statement.
 * @details Parses if-then-else control flow with optional else clause.
 * @param parser The parser context.
 * @return Pointer to an if statement node.
 */
csq_node* parse_if_statement(struct csq_parser* parser);

/**
 * @brief Parses a while loop.
 * @details Parses while loop with condition and body.
 * @param parser The parser context.
 * @return Pointer to a while loop node.
 */
csq_node* parse_while_statement(struct csq_parser* parser);

/**
 * @brief Parses a for loop.
 * @details Parses for loop with variable, iterable, and body.
 * @param parser The parser context.
 * @return Pointer to a for loop node.
 */
csq_node* parse_for_statement(struct csq_parser* parser);

/**
 * @brief Parses a return statement.
 * @details Parses return statement with optional expression.
 * @param parser The parser context.
 * @return Pointer to a return statement node.
 */
csq_node* parse_return_statement(struct csq_parser* parser);

/**
 * @brief Parses a break statement.
 * @details Parses break statement for exiting loops.
 * @param parser The parser context.
 * @return Pointer to a break statement node.
 */
csq_node* parse_break_statement(struct csq_parser* parser);

/**
 * @brief Parses a continue statement.
 * @details Parses continue statement for continuing to next iteration.
 * @param parser The parser context.
 * @return Pointer to a continue statement node.
 */
csq_node* parse_continue_statement(struct csq_parser* parser);

/**
 * @brief Parses an expression statement.
 * @details Parses a statement that is primarily an expression.
 * @param parser The parser context.
 * @return Pointer to an expression statement node.
 */
csq_node* parse_expression_statement(struct csq_parser* parser);

#endif
