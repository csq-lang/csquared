#ifndef CODEGEN_X86_REG_H
#define CODEGEN_X86_REG_H

typedef enum {
  REG_EAX,
  REG_ECX,
  REG_EDX,
  REG_EBX,
  REG_ESP,
  REG_EBP,
  REG_ESI,
  REG_EDI,
  REG_COUNT
} x86_reg;

typedef enum {
  REG_8_AL,
  REG_8_CL,
  REG_8_DL,
  REG_8_BL,
  REG_8_AH,
  REG_8_CH,
  REG_8_DH,
  REG_8_BH
} x86_reg_8;

typedef enum {
  REG_16_AX,
  REG_16_CX,
  REG_16_DX,
  REG_16_BX,
  REG_16_SP,
  REG_16_BP,
  REG_16_SI,
  REG_16_DI
} x86_reg_16;

const char *reg32_name(x86_reg reg);
const char *reg16_name(x86_reg_16 reg);
const char *reg8_name(x86_reg_8 reg);
const char *reg32_name_byte(x86_reg reg);

#endif
