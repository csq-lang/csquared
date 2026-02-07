#ifndef CODEGEN_X86_GEN_CONST_H
#define CODEGEN_X86_GEN_CONST_H

#include <codegen/x86/gen_core.h>
#include <stdbool.h>

long long gen_fold_constant(gen_ctx* ctx, csq_node* node);
bool gen_is_constant(gen_ctx* ctx, csq_node* node);

#endif
