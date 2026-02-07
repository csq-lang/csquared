/**
 * @file ast_printer.h
 * @brief AST and symbol visualization utilities.
 * @details Provides functions for printing and debugging AST structures,
 * types, and symbol table information for inspection and development.
 */

#ifndef UTIL_AST_PRINTER_H
#define UTIL_AST_PRINTER_H

#include <parser/ast.h>
#include <parser/symbtab.h>
#include <stdio.h>

/**
 * @brief Prints the entire AST to output.
 * @details Recursively outputs the complete AST structure in a human-readable format.
 * @param ctx The AST context to print.
 * @param out Output file stream (typically stdout).
 */
void ast_print(const ast_context* ctx, FILE* out);

/**
 * @brief Prints a single AST node.
 * @details Outputs one AST node with proper indentation for the tree structure.
 * @param node The node to print.
 * @param out Output file stream.
 * @param indent The current indentation level.
 */
void ast_print_node(const csq_node* node, FILE* out, int indent);

/**
 * @brief Prints a type structure.
 * @details Outputs the type information in a readable format.
 * @param type The type to print.
 * @param out Output file stream.
 */
void ast_print_type(const csq_type* type, FILE* out);

/**
 * @brief Prints a symbol table entry.
 * @details Outputs symbol information including name, kind, and type.
 * @param sym The symbol to print.
 * @param out Output file stream.
 */
void ast_print_symbol(const struct csq_symbol* sym, FILE* out);

#endif
