#ifndef CODEGEN_X86_GEN_STRING_H
#define CODEGEN_X86_GEN_STRING_H

#include <codegen/x86/gen_core.h>

const char* gen_get_string_label(gen_ctx* ctx);
const char* gen_add_string(gen_ctx* ctx, const char* value);
void gen_collect_strings(gen_ctx* ctx, csq_node* node);

#endif
