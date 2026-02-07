#ifndef CODEGEN_X86_GEN_UTILS_H
#define CODEGEN_X86_GEN_UTILS_H

#include <codegen/x86/gen_core.h>
#include <parser/types.h>

csq_type* resolve_type_spec(csq_node* type_spec_node);
size_t get_type_size(csq_type* type);
int get_string_index(gen_ctx* ctx, const char* value);
void gen_collect_local_vars(gen_ctx* ctx, csq_node* node);

#endif
