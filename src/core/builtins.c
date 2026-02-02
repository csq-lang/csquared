#include "core/builtins.h"
#include "codegen/x86/emit.h"
#include "codegen/x86/gen.h"
#include "codegen/x86/inst.h"
#include "codegen/x86/reg.h"
#include <string.h>

static const builtin_info builtins[] = {
    {"syscall", BUILTIN_SYSCALL, 1, 6},
};

bool builtin_is_builtin(const char *name) {
  for (size_t i = 0; i < sizeof(builtins) / sizeof(builtins[0]); i++) {
    if (strcmp(builtins[i].name, name) == 0) {
      return true;
    }
  }
  return false;
}

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
  rvn_node **args = (rvn_node **)args_ptr;

  const builtin_info *info = builtin_get_info(name);
  if (!info)
    return;

  switch (info->type) {
  case BUILTIN_SYSCALL: {
    if (arg_count < 1)
      return;
    for (int i = arg_count - 1; i >= 0; i--) {
      gen_expression(ctx, args[i]);
      inst_push_reg(ctx->emit, REG_EAX);
    }
    inst_pop_reg(ctx->emit, REG_EAX);
    if (arg_count >= 2)
      inst_pop_reg(ctx->emit, REG_EBX);
    if (arg_count >= 3)
      inst_pop_reg(ctx->emit, REG_ECX);
    if (arg_count >= 4)
      inst_pop_reg(ctx->emit, REG_EDX);
    if (arg_count >= 5)
      inst_pop_reg(ctx->emit, REG_ESI);
    if (arg_count >= 6)
      inst_pop_reg(ctx->emit, REG_EDI);
    emit_raw(ctx->emit, "    int $0x80\n");
    break;
  }
  default:
    break;
  }
}
