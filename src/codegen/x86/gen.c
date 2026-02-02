#include <codegen/x86/gen.h>
#include <codegen/x86/inst.h>
#include <codegen/x86/reg.h>
#include <core/builtins.h>
#include <math.h>
#include <parser/node.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INITIAL_CAPACITY 64

var_table *var_table_create(void) {
  var_table *table = malloc(sizeof(var_table));
  if (!table)
    return NULL;

  table->vars = malloc(sizeof(var_info) * INITIAL_CAPACITY);
  if (!table->vars) {
    free(table);
    return NULL;
  }

  table->count = 0;
  table->capacity = INITIAL_CAPACITY;
  table->next_offset = 0;

  return table;
}

void var_table_free(var_table *table) {
  if (!table)
    return;
  free(table->vars);
  free(table);
}

static size_t get_type_size(rvn_type *type) {
  if (!type)
    return 4;

  switch (type->kind) {
  case TYPE_I8:
  case TYPE_U8:
  case TYPE_BOOL:
    return 1;
  case TYPE_I16:
  case TYPE_U16:
    return 2;
  case TYPE_I32:
  case TYPE_U32:
  case TYPE_F32:
    return 4;
  case TYPE_I64:
  case TYPE_U64:
  case TYPE_F64:
  case TYPE_POINTER:
  case TYPE_STRING:
    return 4;
  default:
    return 4;
  }
}

var_info *var_table_add(var_table *table, const char *name, rvn_type *type) {
  if (!table || !name)
    return NULL;

  if (table->count >= table->capacity) {
    size_t new_capacity = table->capacity * 2;
    var_info *new_vars = realloc(table->vars, sizeof(var_info) * new_capacity);
    if (!new_vars)
      return NULL;
    table->vars = new_vars;
    table->capacity = new_capacity;
  }

  size_t size = get_type_size(type);
  size_t aligned_size = (size + 3) & ~3;
  table->next_offset -= aligned_size;

  var_info *var = &table->vars[table->count];
  var->name = name;
  var->offset = table->next_offset;
  var->type = type;
  var->is_global = false;
  var->is_param = false;

  table->count++;
  return var;
}

var_info *var_table_find(var_table *table, const char *name) {
  if (!table || !name)
    return NULL;

  for (size_t i = 0; i < table->count; i++) {
    if (strcmp(table->vars[i].name, name) == 0) {
      return &table->vars[i];
    }
  }
  return NULL;
}

gen_ctx *gen_create(ast_context *ast) {
  gen_ctx *ctx = malloc(sizeof(gen_ctx));
  if (!ctx)
    return NULL;

  ctx->emit = emit_create_buffer();
  if (!ctx->emit) {
    free(ctx);
    return NULL;
  }

  ctx->data_emit = emit_create_buffer();
  if (!ctx->data_emit) {
    emit_free(ctx->emit);
    free(ctx);
    return NULL;
  }

  ctx->labels = label_gen_create();
  if (!ctx->labels) {
    emit_free(ctx->data_emit);
    emit_free(ctx->emit);
    free(ctx);
    return NULL;
  }

  ctx->frame = frame_create();
  if (!ctx->frame) {
    label_gen_free(ctx->labels);
    emit_free(ctx->data_emit);
    emit_free(ctx->emit);
    free(ctx);
    return NULL;
  }

  ctx->vars = var_table_create();
  if (!ctx->vars) {
    frame_free(ctx->frame);
    label_gen_free(ctx->labels);
    emit_free(ctx->data_emit);
    emit_free(ctx->emit);
    free(ctx);
    return NULL;
  }

  ctx->strings = malloc(sizeof(string_constant) * INITIAL_CAPACITY);
  if (!ctx->strings) {
    var_table_free(ctx->vars);
    frame_free(ctx->frame);
    label_gen_free(ctx->labels);
    emit_free(ctx->data_emit);
    emit_free(ctx->emit);
    free(ctx);
    return NULL;
  }

  ctx->ast = ast;
  ctx->generate_asm = true;
  ctx->output_file = NULL;
  ctx->string_counter = 0;
  ctx->string_count = 0;
  ctx->string_capacity = INITIAL_CAPACITY;
  ctx->diag = NULL;
  ctx->source_path = NULL;
  ctx->source_buffer = NULL;

  return ctx;
}

void gen_free(gen_ctx *ctx) {
  if (!ctx)
    return;
  for (size_t i = 0; i < ctx->string_count; i++) {
    free(ctx->strings[i].label);
    free(ctx->strings[i].value);
  }
  free(ctx->strings);
  var_table_free(ctx->vars);
  frame_free(ctx->frame);
  label_gen_free(ctx->labels);
  emit_free(ctx->data_emit);
  emit_free(ctx->emit);
  free(ctx->output_file);
  free(ctx);
}

void gen_set_output(gen_ctx *ctx, const char *filename) {
  if (!ctx)
    return;
  free(ctx->output_file);
  ctx->output_file = filename ? strdup(filename) : NULL;
}

void gen_set_asm_mode(gen_ctx *ctx, bool asm_mode) {
  if (ctx)
    ctx->generate_asm = asm_mode;
}

int gen_generate(gen_ctx *ctx) {
  if (!ctx || !ctx->ast)
    return -1;

  if (ctx->ast->root) {
    gen_collect_strings(ctx, ctx->ast->root);
  }

  gen_emit_header(ctx);

  if (ctx->string_count > 0) {
    emit_line(ctx->emit, ".section .rodata");
    for (size_t i = 0; i < ctx->string_count; i++) {
      emit_line(ctx->emit, "%s:", ctx->strings[i].label);
      emit_line(ctx->emit, "    .string \"%s\"", ctx->strings[i].value);
    }
    emit_line(ctx->emit, ".text");
    emit_newline(ctx->emit);
  }

  if (ctx->ast->root) {
    gen_program(ctx, ctx->ast->root);
  }

  gen_emit_footer(ctx);

  if (ctx->output_file) {
    return gen_generate_file(ctx, ctx->output_file);
  }

  return 0;
}

int gen_generate_file(gen_ctx *ctx, const char *filename) {
  if (!ctx || !filename)
    return -1;

  FILE *file = fopen(filename, "w");
  if (!file)
    return -1;

  if (ctx->string_count > 0) {
    fprintf(file, ".section .rodata\n");
    for (size_t i = 0; i < ctx->string_count; i++) {
      fprintf(file, "%s:\n", ctx->strings[i].label);
      fprintf(file, "    .string \"%s\"\n", ctx->strings[i].value);
    }
    fprintf(file, "\n");
  }

  emit_write_to_file(ctx->emit, file);
  fclose(file);

  return 0;
}

void gen_emit_header(gen_ctx *ctx) {
  emit_ctx *e = ctx->emit;
  emit_line(e, ".text");
  emit_line(e, ".globl main");
  emit_newline(e);
}

void gen_emit_footer(gen_ctx *ctx) { (void)ctx; }

const char *gen_add_string(gen_ctx *ctx, const char *value) {
  if (!ctx || !value)
    return NULL;

  if (ctx->string_count >= ctx->string_capacity) {
    size_t new_capacity = ctx->string_capacity * 2;
    string_constant *new_strings =
        realloc(ctx->strings, sizeof(string_constant) * new_capacity);
    if (!new_strings)
      return NULL;
    ctx->strings = new_strings;
    ctx->string_capacity = new_capacity;
  }

  static char label[32];
  snprintf(label, sizeof(label), ".str%d", ctx->string_counter++);

  ctx->strings[ctx->string_count].label = strdup(label);
  ctx->strings[ctx->string_count].value = strdup(value);
  ctx->string_count++;

  return ctx->strings[ctx->string_count - 1].label;
}

void gen_collect_strings(gen_ctx *ctx, rvn_node *node) {
  if (!node)
    return;

  switch (node->type) {
  case NODE_LITERAL_STRING:
    gen_add_string(ctx, node->data.literal_string.value);
    break;
  case NODE_PROGRAM:
    for (size_t i = 0; i < node->data.program.statements.count; i++) {
      gen_collect_strings(ctx, node->data.program.statements.items[i]);
    }
    break;
  case NODE_BLOCK:
    for (size_t i = 0; i < node->data.block.statements.count; i++) {
      gen_collect_strings(ctx, node->data.block.statements.items[i]);
    }
    break;
  case NODE_VAR_DECL:
    gen_collect_strings(ctx, node->data.var_decl.init);
    break;
  case NODE_BINARY_OP:
    gen_collect_strings(ctx, node->data.binary.left);
    gen_collect_strings(ctx, node->data.binary.right);
    break;
  case NODE_UNARY_OP:
    gen_collect_strings(ctx, node->data.unary.operand);
    break;
  case NODE_IF:
    gen_collect_strings(ctx, node->data.if_stmt.condition);
    gen_collect_strings(ctx, node->data.if_stmt.then_branch);
    gen_collect_strings(ctx, node->data.if_stmt.else_branch);
    break;
  case NODE_WHILE:
    gen_collect_strings(ctx, node->data.while_stmt.condition);
    gen_collect_strings(ctx, node->data.while_stmt.body);
    break;
  case NODE_RETURN:
    gen_collect_strings(ctx, node->data.return_stmt.value);
    break;
  case NODE_CALL:
    gen_collect_strings(ctx, node->data.call.callee);
    for (size_t i = 0; i < node->data.call.args.count; i++) {
      gen_collect_strings(ctx, node->data.call.args.items[i]);
    }
    break;
  default:
    break;
  }
}

bool gen_is_constant(gen_ctx *ctx, rvn_node *node) {
  (void)ctx;
  if (!node)
    return false;

  switch (node->type) {
  case NODE_LITERAL_INT:
  case NODE_LITERAL_BOOL:
  case NODE_LITERAL_NULL:
    return true;
  case NODE_BINARY_OP:
    return gen_is_constant(ctx, node->data.binary.left) &&
           gen_is_constant(ctx, node->data.binary.right);
  case NODE_UNARY_OP:
    return gen_is_constant(ctx, node->data.unary.operand);
  default:
    return false;
  }
}

long long gen_fold_constant(gen_ctx *ctx, rvn_node *node) {
  (void)ctx;
  if (!node)
    return 0;

  switch (node->type) {
  case NODE_LITERAL_INT:
    return node->data.literal_int.value;
  case NODE_LITERAL_BOOL:
    return node->data.literal_bool.value ? 1 : 0;
  case NODE_LITERAL_NULL:
    return 0;
  case NODE_BINARY_OP: {
    long long left = gen_fold_constant(ctx, node->data.binary.left);
    long long right = gen_fold_constant(ctx, node->data.binary.right);
    switch (node->data.binary.op) {
    case BINOP_ADD:
      return left + right;
    case BINOP_SUB:
      return left - right;
    case BINOP_MUL:
      return left * right;
    case BINOP_DIV:
      return right != 0 ? left / right : 0;
    case BINOP_MOD:
      return right != 0 ? left % right : 0;
    case BINOP_AND:
      return left && right;
    case BINOP_OR:
      return left || right;
    case BINOP_BIT_AND:
      return left & right;
    case BINOP_BIT_OR:
      return left | right;
    case BINOP_BIT_XOR:
      return left ^ right;
    case BINOP_SHL:
      return left << right;
    case BINOP_SHR:
      return left >> right;
    case BINOP_EQ:
      return left == right;
    case BINOP_NE:
      return left != right;
    case BINOP_LT:
      return left < right;
    case BINOP_LE:
      return left <= right;
    case BINOP_GT:
      return left > right;
    case BINOP_GE:
      return left >= right;
    default:
      return 0;
    }
  }
  case NODE_UNARY_OP: {
    long long val = gen_fold_constant(ctx, node->data.unary.operand);
    switch (node->data.unary.op) {
    case UNOP_NEG:
      return -val;
    case UNOP_NOT:
      return !val;
    case UNOP_BIT_NOT:
      return ~val;
    default:
      return val;
    }
  }
  default:
    return 0;
  }
}

void gen_program(gen_ctx *ctx, rvn_node *node) {
  if (!node || node->type != NODE_PROGRAM)
    return;

  for (size_t i = 0; i < node->data.program.statements.count; i++) {
    rvn_node *stmt = node->data.program.statements.items[i];
    if (stmt->type == NODE_FUNCTION_DECL) {
      gen_function(ctx, stmt);
    }
  }

  bool has_main = false;
  for (size_t i = 0; i < node->data.program.statements.count; i++) {
    rvn_node *stmt = node->data.program.statements.items[i];
    if (stmt->type == NODE_FUNCTION_DECL) {
      const char *name = stmt->data.function_decl.name->data.identifier.name;
      if (strcmp(name, "main") == 0) {
        has_main = true;
        break;
      }
    }
  }

  if (!has_main) {
    emit_label(ctx->emit, "main");
    emit_indent(ctx->emit);
    inst_push_reg(ctx->emit, REG_EBP);
    inst_mov_reg_reg(ctx->emit, REG_EBP, REG_ESP);

    int stack_size = 0;
    for (size_t i = 0; i < node->data.program.statements.count; i++) {
      rvn_node *stmt = node->data.program.statements.items[i];
      if (stmt->type == NODE_VAR_DECL) {
        stack_size += 4;
      }
    }
    if (stack_size > 0) {
      inst_sub_reg_imm(ctx->emit, REG_ESP, stack_size);
    }

    for (size_t i = 0; i < node->data.program.statements.count; i++) {
      rvn_node *stmt = node->data.program.statements.items[i];
      if (stmt->type != NODE_FUNCTION_DECL) {
        gen_statement(ctx, stmt);
      }
    }

    inst_mov_reg_imm(ctx->emit, REG_EAX, 0);
    inst_mov_reg_reg(ctx->emit, REG_ESP, REG_EBP);
    inst_pop_reg(ctx->emit, REG_EBP);
    inst_ret(ctx->emit);
    emit_dedent(ctx->emit);
  }

  emit_raw(ctx->emit, ".globl _start\n");
  emit_label(ctx->emit, "_start");
  emit_indent(ctx->emit);
  inst_push_reg(ctx->emit, REG_EBP);
  inst_mov_reg_reg(ctx->emit, REG_EBP, REG_ESP);

  emit_raw(ctx->emit, "    call main\n");

  inst_mov_reg_reg(ctx->emit, REG_EBX, REG_EAX);
  inst_mov_reg_imm(ctx->emit, REG_EAX, 1);
  inst_mov_reg_reg(ctx->emit, REG_ESP, REG_EBP);
  inst_pop_reg(ctx->emit, REG_EBP);
  emit_raw(ctx->emit, "    int $0x80\n");
  emit_dedent(ctx->emit);
}

void gen_statement(gen_ctx *ctx, rvn_node *node) {
  if (!node)
    return;

  switch (node->type) {
  case NODE_FUNCTION_DECL:
    gen_function(ctx, node);
    break;
  case NODE_VAR_DECL:
    gen_var_decl(ctx, node);
    break;
  case NODE_IF:
    gen_if(ctx, node);
    break;
  case NODE_WHILE:
    gen_while(ctx, node);
    break;
  case NODE_RETURN:
    gen_return(ctx, node);
    break;
  case NODE_EXPR_STMT:
    gen_expression(ctx, node->data.expr_stmt.expr);
    break;
  case NODE_BLOCK:
    gen_block(ctx, node);
    break;
  default:
    gen_expression(ctx, node);
    break;
  }
}

void gen_function(gen_ctx *ctx, rvn_node *node) {
  if (!node || node->type != NODE_FUNCTION_DECL)
    return;

  const char *name = node->data.function_decl.name->data.identifier.name;
  emit_label(ctx->emit, name);

  ctx->vars->count = 0;
  ctx->vars->next_offset = 0;

  int param_count = node->data.function_decl.params.count;
  frame_begin_function(ctx->frame, param_count);
  emit_indent(ctx->emit);
  frame_emit_prologue(ctx->frame, ctx->emit);

  char *epilogue_label = label_gen_new(ctx->labels, "epilogue");

  ctx->frame->epilogue_label = epilogue_label;

  for (size_t i = 0; i < node->data.function_decl.params.count; i++) {
    rvn_node *param = node->data.function_decl.params.items[i];
    if (param->type == NODE_PARAM) {
      const char *param_name = param->data.param.name->data.identifier.name;
      rvn_type *param_type = param->type_info;

      int param_offset = 8 + (i * 4);

      var_info *var = var_table_add(ctx->vars, param_name, param_type);
      if (var) {
        var->offset = param_offset;
        var->is_param = true;
      }
    }
  }

  if (node->data.function_decl.body) {
    gen_block(ctx, node->data.function_decl.body);
  }

  emit_label(ctx->emit, epilogue_label);
  frame_emit_epilogue(ctx->frame, ctx->emit);
  emit_dedent(ctx->emit);
  frame_end_function(ctx->frame);
  emit_newline(ctx->emit);
}

void gen_block(gen_ctx *ctx, rvn_node *node) {
  if (!node || node->type != NODE_BLOCK)
    return;

  emit_indent(ctx->emit);
  for (size_t i = 0; i < node->data.block.statements.count; i++) {
    gen_statement(ctx, node->data.block.statements.items[i]);
  }
  emit_dedent(ctx->emit);
}

void gen_if(gen_ctx *ctx, rvn_node *node) {
  if (!node || node->type != NODE_IF)
    return;

  char *else_label = label_gen_new(ctx->labels, "else");
  char *end_label = label_gen_new(ctx->labels, "endif");

  gen_expression(ctx, node->data.if_stmt.condition);
  inst_cmp_reg_imm(ctx->emit, REG_EAX, 0);
  inst_je_label(ctx->emit, else_label);

  emit_indent(ctx->emit);
  gen_statement(ctx, node->data.if_stmt.then_branch);
  emit_dedent(ctx->emit);
  inst_jmp_label(ctx->emit, end_label);

  emit_label(ctx->emit, else_label);
  if (node->data.if_stmt.else_branch) {
    emit_indent(ctx->emit);
    gen_statement(ctx, node->data.if_stmt.else_branch);
    emit_dedent(ctx->emit);
  }

  emit_label(ctx->emit, end_label);
}

void gen_while(gen_ctx *ctx, rvn_node *node) {
  if (!node || node->type != NODE_WHILE)
    return;

  char *start_label = label_gen_new(ctx->labels, "while");
  char *end_label = label_gen_new(ctx->labels, "endwhile");

  emit_label(ctx->emit, start_label);
  gen_expression(ctx, node->data.while_stmt.condition);
  inst_cmp_reg_imm(ctx->emit, REG_EAX, 0);
  inst_je_label(ctx->emit, end_label);

  emit_indent(ctx->emit);
  gen_statement(ctx, node->data.while_stmt.body);
  emit_dedent(ctx->emit);
  inst_jmp_label(ctx->emit, start_label);

  emit_label(ctx->emit, end_label);
}

void gen_return(gen_ctx *ctx, rvn_node *node) {
  if (!node || node->type != NODE_RETURN)
    return;

  if (node->data.return_stmt.value) {
    gen_expression(ctx, node->data.return_stmt.value);
  } else {
    inst_mov_reg_imm(ctx->emit, REG_EAX, 0);
  }

  if (ctx->frame->epilogue_label) {
    inst_jmp_label(ctx->emit, ctx->frame->epilogue_label);
  } else {

    frame_emit_epilogue(ctx->frame, ctx->emit);
  }
}

static int get_string_index(gen_ctx *ctx, const char *value) {
  for (size_t i = 0; i < ctx->string_count; i++) {
    if (strcmp(ctx->strings[i].value, value) == 0) {
      return (int)i;
    }
  }
  return -1;
}

void gen_var_decl(gen_ctx *ctx, rvn_node *node) {
  if (!node || node->type != NODE_VAR_DECL)
    return;

  const char *name = node->data.var_decl.name->data.identifier.name;
  rvn_type *type = node->type_info;
  rvn_node *init = node->data.var_decl.init;

  var_info *var = var_table_add(ctx->vars, name, type);
  if (!var)
    return;

  if (init) {
    if (init->type == NODE_LITERAL_STRING) {
      int idx = get_string_index(ctx, init->data.literal_string.value);
      if (idx >= 0) {
        emit_line(ctx->emit, "movl $%s, %d(%%ebp)", ctx->strings[idx].label,
                  var->offset);
      }
    } else if (init->type == NODE_LITERAL_FLOAT) {
      double dval = init->data.literal_float.value;
      unsigned int ival;
      float fval = (float)dval;
      memcpy(&ival, &fval, sizeof(ival));
      emit_line(ctx->emit, "movl $0x%x, %d(%%ebp)", ival, var->offset);
    } else if (type && (type->kind == TYPE_F32 || type->kind == TYPE_F64)) {
      if (gen_is_constant(ctx, init)) {
        long long val = gen_fold_constant(ctx, init);
        emit_line(ctx->emit, "movl $%d, %d(%%ebp)", (int)val, var->offset);
      } else {
        gen_expression(ctx, init);
        inst_mov_mem_reg(ctx->emit, REG_EBP, var->offset, REG_EAX);
      }
    } else {
      if (gen_is_constant(ctx, init)) {
        long long val = gen_fold_constant(ctx, init);
        emit_line(ctx->emit, "movl $%d, %d(%%ebp)", (int)val, var->offset);
      } else {
        gen_expression(ctx, init);
        inst_mov_mem_reg(ctx->emit, REG_EBP, var->offset, REG_EAX);
      }
    }
  }
}

void gen_expression(gen_ctx *ctx, rvn_node *node) {
  if (!node)
    return;

  switch (node->type) {
  case NODE_BINARY_OP:
    gen_binary_op(ctx, node);
    break;
  case NODE_UNARY_OP:
    gen_unary_op(ctx, node);
    break;
  case NODE_CALL:
    gen_call(ctx, node);
    break;
  case NODE_IDENTIFIER:
    gen_identifier(ctx, node);
    break;
  case NODE_LITERAL_INT:
  case NODE_LITERAL_BOOL:
  case NODE_LITERAL_NULL:
  case NODE_LITERAL_STRING:
    gen_literal(ctx, node);
    break;
  case NODE_INDEX:
    gen_index(ctx, node);
    break;
  case NODE_ARRAY_LITERAL:
    gen_array_literal(ctx, node);
    break;
  default:
    break;
  }
}

void gen_binary_op(gen_ctx *ctx, rvn_node *node) {
  if (!node || node->type != NODE_BINARY_OP)
    return;

  if (node->data.binary.op == BINOP_ADD) {
    bool is_string_concat = false;

    if (node->data.binary.left->type == NODE_LITERAL_STRING ||
        node->data.binary.right->type == NODE_LITERAL_STRING) {
      is_string_concat = true;
    }

    if (!is_string_concat) {
      rvn_type *left_type = node->data.binary.left->type_info;
      rvn_type *right_type = node->data.binary.right->type_info;
      if ((left_type && left_type->kind == TYPE_STRING) ||
          (right_type && right_type->kind == TYPE_STRING)) {
        is_string_concat = true;
      }
    }

    if (is_string_concat) {

      gen_expression(ctx, node->data.binary.left);
      inst_push_reg(ctx->emit, REG_EAX);

      emit_raw(ctx->emit, "    movl %eax, %edi\n");
      emit_raw(ctx->emit, "    xorl %ecx, %ecx\n");
      emit_raw(ctx->emit, "strlen_left_loop:\n");
      emit_raw(ctx->emit, "    movb (%edi), %al\n");
      emit_raw(ctx->emit, "    testb %al, %al\n");
      emit_raw(ctx->emit, "    je strlen_left_done\n");
      emit_raw(ctx->emit, "    incl %ecx\n");
      emit_raw(ctx->emit, "    incl %edi\n");
      emit_raw(ctx->emit, "    jmp strlen_left_loop\n");
      emit_raw(ctx->emit, "strlen_left_done:\n");
      emit_raw(ctx->emit, "    movl %ecx, %ebx\n");

      inst_pop_reg(ctx->emit, REG_EAX);
      inst_push_reg(ctx->emit, REG_EAX);

      gen_expression(ctx, node->data.binary.right);
      inst_push_reg(ctx->emit, REG_EAX);

      emit_raw(ctx->emit, "    movl %eax, %edi\n");
      emit_raw(ctx->emit, "    xorl %ecx, %ecx\n");
      emit_raw(ctx->emit, "strlen_right_loop:\n");
      emit_raw(ctx->emit, "    movb (%edi), %al\n");
      emit_raw(ctx->emit, "    testb %al, %al\n");
      emit_raw(ctx->emit, "    je strlen_right_done\n");
      emit_raw(ctx->emit, "    incl %ecx\n");
      emit_raw(ctx->emit, "    incl %edi\n");
      emit_raw(ctx->emit, "    jmp strlen_right_loop\n");
      emit_raw(ctx->emit, "strlen_right_done:\n");

      inst_add_reg_reg(ctx->emit, REG_ECX, REG_EBX);
      inst_inc_reg(ctx->emit, REG_ECX);

      emit_raw(ctx->emit, "    movl %esp, %edx\n");
      emit_raw(ctx->emit, "    movl $0, %eax\n");
      emit_raw(ctx->emit, "    movl $45, %ebx\n");
      emit_raw(ctx->emit, "    int $0x80\n");
      emit_raw(ctx->emit, "    addl %ecx, %eax\n");
      emit_raw(ctx->emit, "    movl %eax, %edi\n");

      inst_pop_reg(ctx->emit, REG_ECX);
      inst_push_reg(ctx->emit, REG_ECX);

      inst_pop_reg(ctx->emit, REG_EAX);
      inst_push_reg(ctx->emit, REG_EAX);

      emit_raw(ctx->emit, "    movl %eax, %esi\n");
      emit_raw(ctx->emit, "    movl %edi, %edx\n");
      emit_raw(ctx->emit, "strcpy_loop:\n");
      emit_raw(ctx->emit, "    movb (%esi), %al\n");
      emit_raw(ctx->emit, "    movb %al, (%edx)\n");
      emit_raw(ctx->emit, "    testb %al, %al\n");
      emit_raw(ctx->emit, "    je strcpy_done\n");
      emit_raw(ctx->emit, "    incl %esi\n");
      emit_raw(ctx->emit, "    incl %edx\n");
      emit_raw(ctx->emit, "    jmp strcpy_loop\n");
      emit_raw(ctx->emit, "strcpy_done:\n");

      emit_raw(ctx->emit, "    movl %edi, %edx\n");
      emit_raw(ctx->emit, "strcat_find_end:\n");
      emit_raw(ctx->emit, "    movb (%edx), %al\n");
      emit_raw(ctx->emit, "    testb %al, %al\n");
      emit_raw(ctx->emit, "    jne strcat_find_end_next\n");
      emit_raw(ctx->emit, "    jmp strcat_copy_start\n");
      emit_raw(ctx->emit, "strcat_find_end_next:\n");
      emit_raw(ctx->emit, "    incl %edx\n");
      emit_raw(ctx->emit, "    jmp strcat_find_end\n");
      emit_raw(ctx->emit, "strcat_copy_start:\n");

      inst_pop_reg(ctx->emit, REG_ESI);

      emit_raw(ctx->emit, "strcat_copy_loop:\n");
      emit_raw(ctx->emit, "    movb (%esi), %al\n");
      emit_raw(ctx->emit, "    movb %al, (%edx)\n");
      emit_raw(ctx->emit, "    testb %al, %al\n");
      emit_raw(ctx->emit, "    je strcat_done\n");
      emit_raw(ctx->emit, "    incl %esi\n");
      emit_raw(ctx->emit, "    incl %edx\n");
      emit_raw(ctx->emit, "    jmp strcat_copy_loop\n");
      emit_raw(ctx->emit, "strcat_done:\n");

      inst_pop_reg(ctx->emit, REG_EAX);

      inst_mov_reg_reg(ctx->emit, REG_EAX, REG_EDI);
      return;
    }
  }

  if (gen_is_constant(ctx, node)) {
    long long val = gen_fold_constant(ctx, node);
    inst_mov_reg_imm(ctx->emit, REG_EAX, (int)val);
    return;
  }

  gen_expression(ctx, node->data.binary.left);
  inst_push_reg(ctx->emit, REG_EAX);
  gen_expression(ctx, node->data.binary.right);
  inst_mov_reg_reg(ctx->emit, REG_EBX, REG_EAX);
  inst_pop_reg(ctx->emit, REG_EAX);

  switch (node->data.binary.op) {
  case BINOP_ADD:
    inst_add_reg_reg(ctx->emit, REG_EAX, REG_EBX);
    break;
  case BINOP_SUB:
    inst_sub_reg_reg(ctx->emit, REG_EAX, REG_EBX);
    break;
  case BINOP_MUL:
    inst_imul_reg_reg(ctx->emit, REG_EAX, REG_EBX);
    break;
  case BINOP_DIV:
    inst_cdq(ctx->emit);
    inst_idiv_reg(ctx->emit, REG_EBX);
    break;
  case BINOP_MOD:
    inst_cdq(ctx->emit);
    inst_idiv_reg(ctx->emit, REG_EBX);
    inst_mov_reg_reg(ctx->emit, REG_EAX, REG_EDX);
    break;
  case BINOP_AND:
    inst_and_reg_reg(ctx->emit, REG_EAX, REG_EBX);
    break;
  case BINOP_OR:
    inst_or_reg_reg(ctx->emit, REG_EAX, REG_EBX);
    break;
  case BINOP_BIT_AND:
    inst_and_reg_reg(ctx->emit, REG_EAX, REG_EBX);
    break;
  case BINOP_BIT_OR:
    inst_or_reg_reg(ctx->emit, REG_EAX, REG_EBX);
    break;
  case BINOP_BIT_XOR:
    inst_xor_reg_reg(ctx->emit, REG_EAX, REG_EBX);
    break;
  case BINOP_SHL:
    inst_shl_reg_imm(ctx->emit, REG_EAX, 1);
    break;
  case BINOP_SHR:
    inst_shr_reg_imm(ctx->emit, REG_EAX, 1);
    break;
  case BINOP_EQ:
    inst_cmp_reg_reg(ctx->emit, REG_EAX, REG_EBX);
    inst_sete_reg(ctx->emit, REG_EAX);
    inst_and_reg_imm(ctx->emit, REG_EAX, 0xFF);
    break;
  case BINOP_NE:
    inst_cmp_reg_reg(ctx->emit, REG_EAX, REG_EBX);
    inst_setne_reg(ctx->emit, REG_EAX);
    inst_and_reg_imm(ctx->emit, REG_EAX, 0xFF);
    break;
  case BINOP_LT:
    inst_cmp_reg_reg(ctx->emit, REG_EAX, REG_EBX);
    inst_setl_reg(ctx->emit, REG_EAX);
    inst_and_reg_imm(ctx->emit, REG_EAX, 0xFF);
    break;
  case BINOP_LE:
    inst_cmp_reg_reg(ctx->emit, REG_EAX, REG_EBX);
    inst_setle_reg(ctx->emit, REG_EAX);
    inst_and_reg_imm(ctx->emit, REG_EAX, 0xFF);
    break;
  case BINOP_GT:
    inst_cmp_reg_reg(ctx->emit, REG_EAX, REG_EBX);
    inst_setg_reg(ctx->emit, REG_EAX);
    inst_and_reg_imm(ctx->emit, REG_EAX, 0xFF);
    break;
  case BINOP_GE:
    inst_cmp_reg_reg(ctx->emit, REG_EAX, REG_EBX);
    inst_setge_reg(ctx->emit, REG_EAX);
    inst_and_reg_imm(ctx->emit, REG_EAX, 0xFF);
    break;
  default:
    break;
  }
}

void gen_unary_op(gen_ctx *ctx, rvn_node *node) {
  if (!node || node->type != NODE_UNARY_OP)
    return;

  gen_expression(ctx, node->data.unary.operand);

  switch (node->data.unary.op) {
  case UNOP_NEG:
    inst_neg_reg(ctx->emit, REG_EAX);
    break;
  case UNOP_NOT:
    inst_test_reg_reg(ctx->emit, REG_EAX, REG_EAX);
    inst_setne_reg(ctx->emit, REG_EAX);
    inst_xor_reg_imm(ctx->emit, REG_EAX, 1);
    break;
  case UNOP_BIT_NOT:
    inst_not_reg(ctx->emit, REG_EAX);
    break;
  default:
    break;
  }
}

void gen_call(gen_ctx *ctx, rvn_node *node) {
  if (!node || node->type != NODE_CALL)
    return;

  if (node->data.call.callee->type == NODE_IDENTIFIER) {
    const char *name = node->data.call.callee->data.identifier.name;
    if (builtin_is_builtin(name)) {

      builtin_gen_call(ctx, name, node->data.call.args.items,
                       node->data.call.args.count);
      return;
    }
  }

  for (int i = node->data.call.args.count - 1; i >= 0; i--) {
    gen_expression(ctx, node->data.call.args.items[i]);
    inst_push_reg(ctx->emit, REG_EAX);
  }

  if (node->data.call.callee->type == NODE_IDENTIFIER) {
    const char *name = node->data.call.callee->data.identifier.name;
    inst_call_label(ctx->emit, name);
  }

  if (node->data.call.args.count > 0) {
    inst_add_reg_imm(ctx->emit, REG_ESP, node->data.call.args.count * 4);
  }
}

void gen_identifier(gen_ctx *ctx, rvn_node *node) {
  if (!node || node->type != NODE_IDENTIFIER)
    return;

  const char *name = node->data.identifier.name;
  var_info *var = var_table_find(ctx->vars, name);

  if (var) {
    if (var->is_param) {

      inst_mov_reg_mem(ctx->emit, REG_EAX, REG_EBP, var->offset);
    } else {

      inst_mov_reg_mem(ctx->emit, REG_EAX, REG_EBP, var->offset);
    }
  } else {

    if (ctx->diag && ctx->source_path) {
      diag_report_error(ctx->diag, DIAG_ERROR_UNDEFINED_IDENTIFIER,
                        ctx->source_path, node->line, node->column,
                        strlen(name), NULL, "Undefined identifier");
    }
  }
}

void gen_literal(gen_ctx *ctx, rvn_node *node) {
  if (!node)
    return;

  switch (node->type) {
  case NODE_LITERAL_INT:
    inst_mov_reg_imm(ctx->emit, REG_EAX, (int)node->data.literal_int.value);
    break;
  case NODE_LITERAL_BOOL:
    inst_mov_reg_imm(ctx->emit, REG_EAX, node->data.literal_bool.value ? 1 : 0);
    break;
  case NODE_LITERAL_NULL:
    inst_mov_reg_imm(ctx->emit, REG_EAX, 0);
    break;
  case NODE_LITERAL_STRING: {
    int idx = get_string_index(ctx, node->data.literal_string.value);
    if (idx < 0) {
      const char *label = gen_add_string(ctx, node->data.literal_string.value);
      if (label) {
        emit_line(ctx->emit, "movl $%s, %%eax", label);
      }
    } else {
      emit_line(ctx->emit, "movl $%s, %%eax", ctx->strings[idx].label);
    }
    break;
  }
  default:
    break;
  }
}

void gen_array_literal(gen_ctx *ctx, rvn_node *node) {
  if (!node || node->type != NODE_ARRAY_LITERAL)
    return;

  size_t count = node->data.array_literal.elements.count;
  if (count == 0) {
    inst_mov_reg_imm(ctx->emit, REG_EAX, 0);
    return;
  }

  size_t elem_size = 4;
  size_t total_size = count * elem_size;

  emit_line(ctx->emit, "movl $0, %%eax");
  emit_line(ctx->emit, "movl $45, %%ebx");
  emit_line(ctx->emit, "int $0x80");
  emit_line(ctx->emit, "movl %%eax, %%edi");
  emit_line(ctx->emit, "addl $%zu, %%eax", total_size);
  emit_line(ctx->emit, "movl %%eax, %%ebx");
  emit_line(ctx->emit, "movl $45, %%eax");
  emit_line(ctx->emit, "int $0x80");

  for (size_t i = 0; i < count; i++) {
    gen_expression(ctx, node->data.array_literal.elements.items[i]);
    emit_line(ctx->emit, "movl %%eax, %zu(%%edi)", i * elem_size);
  }

  inst_mov_reg_reg(ctx->emit, REG_EAX, REG_EDI);
}

void gen_index(gen_ctx *ctx, rvn_node *node) {
  if (!node || node->type != NODE_INDEX)
    return;

  gen_expression(ctx, node->data.index.object);
  inst_push_reg(ctx->emit, REG_EAX);

  gen_expression(ctx, node->data.index.index);
  inst_mov_reg_reg(ctx->emit, REG_EBX, REG_EAX);

  inst_pop_reg(ctx->emit, REG_EAX);

  emit_line(ctx->emit, "movl (%%eax, %%ebx, 4), %%eax");
}
