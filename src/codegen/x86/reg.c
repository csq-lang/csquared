/**
 * @file reg.c
 * @brief x86 register definitions and naming
 * @details Provides register names and conversion functions for AT&T syntax.
 */

#include <codegen/x86/reg.h>

static const char *reg32_names[] = {"%eax", "%ecx", "%edx", "%ebx",
                                    "%esp", "%ebp", "%esi", "%edi"};

static const char *reg16_names[] = {"%ax", "%cx", "%dx", "%bx",
                                    "%sp", "%bp", "%si", "%di"};

static const char *reg8_names[] = {"%al", "%cl", "%dl", "%bl",
                                   "%ah", "%ch", "%dh", "%bh"};

/**
 * @brief Get 32-bit register name
 * @param reg Register enumeration
 * @return AT&T syntax register name string
 */
const char *reg32_name(x86_reg reg) {
  if (reg >= REG_COUNT)
    return "%eax";
  return reg32_names[reg];
}

const char *reg16_name(x86_reg_16 reg) {
  if (reg > REG_16_DI)
    return "%ax";
  return reg16_names[reg];
}

const char *reg8_name(x86_reg_8 reg) {
  if (reg > REG_8_BH)
    return "%al";
  return reg8_names[reg];
}

const char *reg32_name_byte(x86_reg reg) {
  if (reg >= REG_COUNT)
    return "%al";
  return reg8_names[reg];
}
