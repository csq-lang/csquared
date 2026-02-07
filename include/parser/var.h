/**
 * @file var.h
 * @brief Variable and constant declaration parsing for the C² parser.
 * @details Provides functions for parsing variable and constant declarations with type and initialization.
 */

#ifndef PARSER_VAR_H
#define PARSER_VAR_H

#include <parser/node.h>
#include <stdbool.h>

struct csq_parser;

/**
 * @brief Parses a variable or constant declaration.
 * @details Parses variable or constant declaration with optional type annotation and initializer.
 * @param parser The parser context.
 * @param is_const True if parsing a constant declaration, false for variable.
 * @return Pointer to a variable or constant declaration node.
 */
csq_node* parse_var_declaration(struct csq_parser* parser, bool is_const);

#endif
