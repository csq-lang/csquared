#ifndef UTIL_AST_PRINTER_H
#define UTIL_AST_PRINTER_H

#include <parser/ast.h>
#include <parser/symbtab.h>
#include <stdio.h>

void ast_print(const ast_context *ctx, FILE *out);
void ast_print_node(const rvn_node *node, FILE *out, int indent);
void ast_print_type(const rvn_type *type, FILE *out);
void ast_print_symbol(const struct rvn_symbol *sym, FILE *out);

#endif
