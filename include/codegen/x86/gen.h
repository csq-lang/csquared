#ifndef CODEGEN_X86_GEN_H
#define CODEGEN_X86_GEN_H

#include <codegen/x86/emit.h>
#include <codegen/x86/frame.h>
#include <codegen/x86/label.h>
#include <core/diag.h>
#include <parser/ast.h>
#include <parser/types.h>
#include <stdbool.h>

typedef struct {
  const char *name;
  int offset;
  rvn_type *type;
  bool is_global;
  bool is_param;
} var_info;

typedef struct {
  var_info *vars;
  size_t count;
  size_t capacity;
  int next_offset;
} var_table;

typedef struct {
  char *label;
  char *value;
} string_constant;

typedef struct {
  emit_ctx *emit;
  emit_ctx *data_emit;
  label_gen *labels;
  frame_ctx *frame;
  ast_context *ast;
  var_table *vars;
  string_constant *strings;
  size_t string_count;
  size_t string_capacity;
  bool generate_asm;
  char *output_file;
  int string_counter;
  DiagReporter *diag;
  const char *source_path;
  const char *source_buffer;
} gen_ctx;

gen_ctx *gen_create(ast_context *ast);
void gen_free(gen_ctx *ctx);

void gen_set_output(gen_ctx *ctx, const char *filename);
void gen_set_asm_mode(gen_ctx *ctx, bool asm_mode);

int gen_generate(gen_ctx *ctx);
int gen_generate_file(gen_ctx *ctx, const char *filename);

void gen_collect_strings(gen_ctx *ctx, rvn_node *node);
void gen_program(gen_ctx *ctx, rvn_node *node);
void gen_function(gen_ctx *ctx, rvn_node *node);
void gen_block(gen_ctx *ctx, rvn_node *node);
void gen_statement(gen_ctx *ctx, rvn_node *node);
void gen_expression(gen_ctx *ctx, rvn_node *node);
void gen_if(gen_ctx *ctx, rvn_node *node);
void gen_while(gen_ctx *ctx, rvn_node *node);
void gen_return(gen_ctx *ctx, rvn_node *node);
void gen_var_decl(gen_ctx *ctx, rvn_node *node);
void gen_binary_op(gen_ctx *ctx, rvn_node *node);
void gen_unary_op(gen_ctx *ctx, rvn_node *node);
void gen_call(gen_ctx *ctx, rvn_node *node);
void gen_identifier(gen_ctx *ctx, rvn_node *node);
void gen_literal(gen_ctx *ctx, rvn_node *node);

void gen_emit_header(gen_ctx *ctx);
void gen_emit_footer(gen_ctx *ctx);
void gen_emit_data_section(gen_ctx *ctx);

void gen_index(gen_ctx *ctx, rvn_node *node);
void gen_array_literal(gen_ctx *ctx, rvn_node *node);

var_table *var_table_create(void);
void var_table_free(var_table *table);
var_info *var_table_add(var_table *table, const char *name, rvn_type *type);
var_info *var_table_find(var_table *table, const char *name);

long long gen_fold_constant(gen_ctx *ctx, rvn_node *node);
bool gen_is_constant(gen_ctx *ctx, rvn_node *node);

const char *gen_get_string_label(gen_ctx *ctx);

#endif
