#include <codegen/x86/inst.h>
#include <stdio.h>

void inst_mov_reg_reg(emit_ctx *ctx, x86_reg dst, x86_reg src) {
  emit_instruction(ctx, "movl", "%s, %s", reg32_name(src), reg32_name(dst));
}

void inst_mov_reg_imm(emit_ctx *ctx, x86_reg dst, int imm) {
  emit_instruction(ctx, "movl", "$%d, %s", imm, reg32_name(dst));
}

void inst_mov_reg_mem(emit_ctx *ctx, x86_reg dst, x86_reg base, int offset) {
  if (offset == 0)
    emit_instruction(ctx, "movl", "(%s), %s", reg32_name(base),
                     reg32_name(dst));
  else
    emit_instruction(ctx, "movl", "%d(%s), %s", offset, reg32_name(base),
                     reg32_name(dst));
}

void inst_mov_mem_reg(emit_ctx *ctx, x86_reg base, int offset, x86_reg src) {
  if (offset == 0)
    emit_instruction(ctx, "movl", "%s, (%s)", reg32_name(src),
                     reg32_name(base));
  else
    emit_instruction(ctx, "movl", "%s, %d(%s)", reg32_name(src), offset,
                     reg32_name(base));
}

void inst_mov_reg_mem_indexed(emit_ctx *ctx, x86_reg dst, x86_reg base,
                              x86_reg index, int scale, int offset) {
  emit_instruction(ctx, "movl", "%d(%s,%s,%d), %s", offset, reg32_name(base),
                   reg32_name(index), scale, reg32_name(dst));
}

void inst_push_reg(emit_ctx *ctx, x86_reg reg) {
  emit_instruction(ctx, "pushl", "%s", reg32_name(reg));
}

void inst_push_imm(emit_ctx *ctx, int imm) {
  emit_instruction(ctx, "pushl", "$%d", imm);
}

void inst_pop_reg(emit_ctx *ctx, x86_reg reg) {
  emit_instruction(ctx, "popl", "%s", reg32_name(reg));
}

void inst_add_reg_reg(emit_ctx *ctx, x86_reg dst, x86_reg src) {
  emit_instruction(ctx, "addl", "%s, %s", reg32_name(src), reg32_name(dst));
}

void inst_add_reg_imm(emit_ctx *ctx, x86_reg dst, int imm) {
  emit_instruction(ctx, "addl", "$%d, %s", imm, reg32_name(dst));
}

void inst_sub_reg_reg(emit_ctx *ctx, x86_reg dst, x86_reg src) {
  emit_instruction(ctx, "subl", "%s, %s", reg32_name(src), reg32_name(dst));
}

void inst_sub_reg_imm(emit_ctx *ctx, x86_reg dst, int imm) {
  emit_instruction(ctx, "subl", "$%d, %s", imm, reg32_name(dst));
}

void inst_imul_reg_reg(emit_ctx *ctx, x86_reg dst, x86_reg src) {
  emit_instruction(ctx, "imull", "%s, %s", reg32_name(src), reg32_name(dst));
}

void inst_idiv_reg(emit_ctx *ctx, x86_reg divisor) {
  emit_instruction(ctx, "idivl", "%s", reg32_name(divisor));
}

void inst_and_reg_reg(emit_ctx *ctx, x86_reg dst, x86_reg src) {
  emit_instruction(ctx, "andl", "%s, %s", reg32_name(src), reg32_name(dst));
}

void inst_and_reg_imm(emit_ctx *ctx, x86_reg dst, int imm) {
  emit_instruction(ctx, "andl", "$%d, %s", imm, reg32_name(dst));
}

void inst_or_reg_reg(emit_ctx *ctx, x86_reg dst, x86_reg src) {
  emit_instruction(ctx, "orl", "%s, %s", reg32_name(src), reg32_name(dst));
}

void inst_xor_reg_reg(emit_ctx *ctx, x86_reg dst, x86_reg src) {
  emit_instruction(ctx, "xorl", "%s, %s", reg32_name(src), reg32_name(dst));
}

void inst_xor_reg_imm(emit_ctx *ctx, x86_reg dst, int imm) {
  emit_instruction(ctx, "xorl", "$%d, %s", imm, reg32_name(dst));
}

void inst_not_reg(emit_ctx *ctx, x86_reg reg) {
  emit_instruction(ctx, "notl", "%s", reg32_name(reg));
}

void inst_neg_reg(emit_ctx *ctx, x86_reg reg) {
  emit_instruction(ctx, "negl", "%s", reg32_name(reg));
}

void inst_shl_reg_imm(emit_ctx *ctx, x86_reg reg, int imm) {
  emit_instruction(ctx, "shll", "$%d, %s", imm, reg32_name(reg));
}

void inst_shr_reg_imm(emit_ctx *ctx, x86_reg reg, int imm) {
  emit_instruction(ctx, "shrl", "$%d, %s", imm, reg32_name(reg));
}

void inst_cmp_reg_reg(emit_ctx *ctx, x86_reg left, x86_reg right) {
  emit_instruction(ctx, "cmpl", "%s, %s", reg32_name(right), reg32_name(left));
}

void inst_cmp_reg_imm(emit_ctx *ctx, x86_reg left, int imm) {
  emit_instruction(ctx, "cmpl", "$%d, %s", imm, reg32_name(left));
}

void inst_test_reg_reg(emit_ctx *ctx, x86_reg left, x86_reg right) {
  emit_instruction(ctx, "testl", "%s, %s", reg32_name(right), reg32_name(left));
}

void inst_jmp_label(emit_ctx *ctx, const char *label) {
  emit_instruction(ctx, "jmp", "%s", label);
}

void inst_je_label(emit_ctx *ctx, const char *label) {
  emit_instruction(ctx, "je", "%s", label);
}

void inst_jne_label(emit_ctx *ctx, const char *label) {
  emit_instruction(ctx, "jne", "%s", label);
}

void inst_jl_label(emit_ctx *ctx, const char *label) {
  emit_instruction(ctx, "jl", "%s", label);
}

void inst_jle_label(emit_ctx *ctx, const char *label) {
  emit_instruction(ctx, "jle", "%s", label);
}

void inst_jg_label(emit_ctx *ctx, const char *label) {
  emit_instruction(ctx, "jg", "%s", label);
}

void inst_jge_label(emit_ctx *ctx, const char *label) {
  emit_instruction(ctx, "jge", "%s", label);
}

void inst_call_label(emit_ctx *ctx, const char *label) {
  emit_instruction(ctx, "call", "%s", label);
}

void inst_call_reg(emit_ctx *ctx, x86_reg reg) {
  emit_instruction(ctx, "call", "*%s", reg32_name(reg));
}

void inst_ret(emit_ctx *ctx) { emit_instruction(ctx, "ret", NULL); }

void inst_lea(emit_ctx *ctx, x86_reg dst, x86_reg base, int offset) {
  if (offset == 0) {
    emit_instruction(ctx, "leal", "(%s), %s", reg32_name(base),
                     reg32_name(dst));
  } else {
    emit_instruction(ctx, "leal", "%d(%s), %s", offset, reg32_name(base),
                     reg32_name(dst));
  }
}

void inst_inc_reg(emit_ctx *ctx, x86_reg reg) {
  emit_instruction(ctx, "incl", "%s", reg32_name(reg));
}

void inst_dec_reg(emit_ctx *ctx, x86_reg reg) {
  emit_instruction(ctx, "decl", "%s", reg32_name(reg));
}

void inst_nop(emit_ctx *ctx) { emit_instruction(ctx, "nop", NULL); }

void inst_cdq(emit_ctx *ctx) { emit_instruction(ctx, "cdq", NULL); }

void inst_sete_reg(emit_ctx *ctx, x86_reg reg) {
  emit_instruction(ctx, "sete", "%s", reg32_name_byte(reg));
}

void inst_setne_reg(emit_ctx *ctx, x86_reg reg) {
  emit_instruction(ctx, "setne", "%s", reg32_name_byte(reg));
}

void inst_setl_reg(emit_ctx *ctx, x86_reg reg) {
  emit_instruction(ctx, "setl", "%s", reg32_name_byte(reg));
}

void inst_setle_reg(emit_ctx *ctx, x86_reg reg) {
  emit_instruction(ctx, "setle", "%s", reg32_name_byte(reg));
}

void inst_setg_reg(emit_ctx *ctx, x86_reg reg) {
  emit_instruction(ctx, "setg", "%s", reg32_name_byte(reg));
}

void inst_setge_reg(emit_ctx *ctx, x86_reg reg) {
  emit_instruction(ctx, "setge", "%s", reg32_name_byte(reg));
}
