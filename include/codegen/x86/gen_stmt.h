#ifndef CODEGEN_X86_GEN_STMT_H
#define CODEGEN_X86_GEN_STMT_H

#include <codegen/x86/gen_core.h>

void gen_program(gen_ctx* ctx, csq_node* node);
void gen_function(gen_ctx* ctx, csq_node* node);
void gen_block(gen_ctx* ctx, csq_node* node);
void gen_statement(gen_ctx* ctx, csq_node* node);

void gen_if(gen_ctx* ctx, csq_node* node);
void gen_while(gen_ctx* ctx, csq_node* node);
void gen_return(gen_ctx* ctx, csq_node* node);
void gen_break(gen_ctx* ctx, csq_node* node);
void gen_continue(gen_ctx* ctx, csq_node* node);

#endif
