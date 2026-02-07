/**
 * @file func.h
 * @brief Function declaration parsing for the C² parser.
 * @details Provides functions for parsing function and method declarations.
 */

#ifndef PARSER_FUNC_H
#define PARSER_FUNC_H

#include <parser/node.h>

struct csq_parser;

/**
 * @brief Parses a function declaration.
 * @details Parses function declaration including name, parameters, return type, and body.
 * @param parser The parser context.
 * @return Pointer to a function declaration node.
 */
csq_node* parse_function_declaration(struct csq_parser* parser);

/**
 * @brief Parses a function declaration (alternative).
 * @details Alternative function for parsing function declarations.
 * @param parser The parser context.
 * @return Pointer to a function declaration node.
 */
csq_node* parse_func_declaration(struct csq_parser* parser);

#endif
