/**
 * @file builtins.c
 * @brief Built-in function support
 * @details Implements code generation for compiler built-in functions
 * like syscall wrappers.
 */

#include "core/builtins.h"
#include "codegen/x86/emit.h"
#include "codegen/x86/gen.h"
#include "codegen/x86/inst.h"
#include "codegen/x86/reg.h"
#include <string.h>

static const builtin_info builtins[] = {
    {"__builtin_syscall", BUILTIN_SYSCALL, 1, 6},
};

/**
 * @brief Check if name is a built-in function
 * @param name Function name
 * @return True if name matches a built-in
 */
bool builtin_is_builtin(const char *name) {
  for (size_t i = 0; i < sizeof(builtins) / sizeof(builtins[0]); i++) {
    if (strcmp(builtins[i].name, name) == 0) {
      return true;
    }
  }
  return false;
}

/**
 * @brief Get built-in function information
 * @param name Function name
 * @return Pointer to builtin info, or NULL if not found
 */
const builtin_info *builtin_get_info(const char *name) {
  for (size_t i = 0; i < sizeof(builtins) / sizeof(builtins[0]); i++) {
    if (strcmp(builtins[i].name, name) == 0) {
      return &builtins[i];
    }
  }
  return NULL;
}

void builtin_gen_call(void *ctx_ptr, const char *name, void *args_ptr,
                      int arg_count) {
  gen_ctx *ctx = (gen_ctx *)ctx_ptr;
  csq_node **args = (csq_node **)args_ptr;

  const builtin_info *info = builtin_get_info(name);
  if (!info)
    return;

  switch (info->type) {
  case BUILTIN_SYSCALL: {
    if (arg_count < 1)
      return;
    x86_reg arg_regs[] = {REG_EAX, REG_EBX, REG_ECX, REG_EDX, REG_ESI, REG_EDI};

    for (int i = arg_count - 1; i >= 0; i--) {
      if (args[i]->type == NODE_LITERAL_INT) {
        int val = (int)args[i]->data.literal_int.value;
        inst_mov_reg_imm(ctx->emit, arg_regs[i], val);
      } else {
        gen_expression(ctx, args[i]);
        if (arg_regs[i] != REG_EAX) {
          inst_mov_reg_reg(ctx->emit, arg_regs[i], REG_EAX);
        }
      }
    }

    emit_line(ctx->emit, "int $0x80");
    break;
  }
  default:
    break;
  }
}
