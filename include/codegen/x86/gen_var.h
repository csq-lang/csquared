#ifndef CODEGEN_X86_GEN_VAR_H
#define CODEGEN_X86_GEN_VAR_H

#include <codegen/x86/gen_core.h>
#include <parser/types.h>

var_table* var_table_create(void);
void var_table_free(var_table* table);
var_info* var_table_add(var_table* table, const char* name, csq_type* type);
var_info* var_table_find(var_table* table, const char* name);

void gen_var_decl(gen_ctx* ctx, csq_node* node);

#endif
