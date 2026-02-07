#ifndef CODEGEN_X86_GEN_UNARY_H
#define CODEGEN_X86_GEN_UNARY_H

#include <codegen/x86/gen_core.h>

void gen_unary_op(gen_ctx* ctx, csq_node* node);
void gen_call(gen_ctx* ctx, csq_node* node);

#endif
