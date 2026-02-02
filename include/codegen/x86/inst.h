#ifndef CODEGEN_X86_INST_H
#define CODEGEN_X86_INST_H

#include <codegen/x86/emit.h>
#include <codegen/x86/reg.h>
#include <stdbool.h>

typedef enum {
  INST_MOV,
  INST_PUSH,
  INST_POP,
  INST_ADD,
  INST_SUB,
  INST_MUL,
  INST_DIV,
  INST_AND,
  INST_OR,
  INST_XOR,
  INST_NOT,
  INST_NEG,
  INST_SHL,
  INST_SHR,
  INST_CMP,
  INST_TEST,
  INST_JMP,
  INST_JE,
  INST_JNE,
  INST_JL,
  INST_JLE,
  INST_JG,
  INST_JGE,
  INST_JB,
  INST_JBE,
  INST_JA,
  INST_JAE,
  INST_CALL,
  INST_RET,
  INST_LEA,
  INST_INC,
  INST_DEC,
  INST_NOP,
  INST_SETE,
  INST_SETNE,
  INST_SETL,
  INST_SETLE,
  INST_SETG,
  INST_SETGE,
  INST_SETB,
  INST_SETBE,
  INST_SETA,
  INST_SETAE,
  INST_CDQ,
  INST_COUNT
} x86_inst_type;

void inst_mov_reg_reg(emit_ctx *ctx, x86_reg dst, x86_reg src);
void inst_mov_reg_imm(emit_ctx *ctx, x86_reg dst, int imm);
void inst_mov_reg_mem(emit_ctx *ctx, x86_reg dst, x86_reg base, int offset);
void inst_mov_mem_reg(emit_ctx *ctx, x86_reg base, int offset, x86_reg src);
void inst_mov_reg_mem_indexed(emit_ctx *ctx, x86_reg dst, x86_reg base,
                              x86_reg index, int scale, int offset);

void inst_push_reg(emit_ctx *ctx, x86_reg reg);
void inst_push_imm(emit_ctx *ctx, int imm);
void inst_pop_reg(emit_ctx *ctx, x86_reg reg);

void inst_add_reg_reg(emit_ctx *ctx, x86_reg dst, x86_reg src);
void inst_add_reg_imm(emit_ctx *ctx, x86_reg dst, int imm);
void inst_sub_reg_reg(emit_ctx *ctx, x86_reg dst, x86_reg src);
void inst_sub_reg_imm(emit_ctx *ctx, x86_reg dst, int imm);

void inst_imul_reg_reg(emit_ctx *ctx, x86_reg dst, x86_reg src);
void inst_idiv_reg(emit_ctx *ctx, x86_reg divisor);

void inst_and_reg_reg(emit_ctx *ctx, x86_reg dst, x86_reg src);
void inst_and_reg_imm(emit_ctx *ctx, x86_reg dst, int imm);
void inst_or_reg_reg(emit_ctx *ctx, x86_reg dst, x86_reg src);
void inst_xor_reg_reg(emit_ctx *ctx, x86_reg dst, x86_reg src);
void inst_xor_reg_imm(emit_ctx *ctx, x86_reg dst, int imm);
void inst_not_reg(emit_ctx *ctx, x86_reg reg);
void inst_neg_reg(emit_ctx *ctx, x86_reg reg);

void inst_shl_reg_imm(emit_ctx *ctx, x86_reg reg, int imm);
void inst_shr_reg_imm(emit_ctx *ctx, x86_reg reg, int imm);

void inst_cmp_reg_reg(emit_ctx *ctx, x86_reg left, x86_reg right);
void inst_cmp_reg_imm(emit_ctx *ctx, x86_reg left, int imm);
void inst_test_reg_reg(emit_ctx *ctx, x86_reg left, x86_reg right);

void inst_jmp_label(emit_ctx *ctx, const char *label);
void inst_je_label(emit_ctx *ctx, const char *label);
void inst_jne_label(emit_ctx *ctx, const char *label);
void inst_jl_label(emit_ctx *ctx, const char *label);
void inst_jle_label(emit_ctx *ctx, const char *label);
void inst_jg_label(emit_ctx *ctx, const char *label);
void inst_jge_label(emit_ctx *ctx, const char *label);

void inst_call_label(emit_ctx *ctx, const char *label);
void inst_call_reg(emit_ctx *ctx, x86_reg reg);
void inst_ret(emit_ctx *ctx);

void inst_lea(emit_ctx *ctx, x86_reg dst, x86_reg base, int offset);

void inst_inc_reg(emit_ctx *ctx, x86_reg reg);
void inst_dec_reg(emit_ctx *ctx, x86_reg reg);

void inst_nop(emit_ctx *ctx);
void inst_cdq(emit_ctx *ctx);

void inst_sete_reg(emit_ctx *ctx, x86_reg reg);
void inst_setne_reg(emit_ctx *ctx, x86_reg reg);
void inst_setl_reg(emit_ctx *ctx, x86_reg reg);
void inst_setle_reg(emit_ctx *ctx, x86_reg reg);
void inst_setg_reg(emit_ctx *ctx, x86_reg reg);
void inst_setge_reg(emit_ctx *ctx, x86_reg reg);

#endif
