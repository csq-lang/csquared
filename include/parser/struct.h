/**
 * @file struct.h
 * @brief Type specification and struct parsing for the C² parser.
 * @details Provides functions for parsing type specifications and composite type declarations.
 */

#ifndef PARSER_STRUCT_H
#define PARSER_STRUCT_H

#include <parser/node.h>

struct csq_parser;

/**
 * @brief Parses a type specification.
 * @details Parses type specifications including primitive types, arrays, and complex types.
 * @param parser The parser context.
 * @return Pointer to a type specification node.
 */
csq_node* parse_type_spec(struct csq_parser* parser);

#endif
