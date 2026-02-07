#ifndef CODEGEN_X86_GEN_EXPR_H
#define CODEGEN_X86_GEN_EXPR_H

#include <codegen/x86/gen_core.h>

void gen_expression(gen_ctx* ctx, csq_node* node);
void gen_identifier(gen_ctx* ctx, csq_node* node);
void gen_literal(gen_ctx* ctx, csq_node* node);

void gen_index(gen_ctx* ctx, csq_node* node);
void gen_array_literal(gen_ctx* ctx, csq_node* node);

#endif
