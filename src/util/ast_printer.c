/**
 * @file ast_printer.c
 * @brief AST visualization and debugging
 * @details Provides functions to print AST structures in a readable
 * hierarchical format for debugging and development.
 */

#include <string.h>
#include <util/ast_printer.h>

/**
 * @brief Print indentation for hierarchical display
 * @param out Output file stream
 * @param indent Number of indentation levels
 */
static void print_indent(FILE *out, int indent) {
  for (int i = 0; i < indent; i++) {
    fprintf(out, "  ");
  }
}

static void print_node_list(const node_list *list, FILE *out, int indent,
                            const char *label);

/**
 * @brief Print AST node and children
 * @param node Node to print
 * @param out Output file stream
 * @param indent Current indentation level
 */
void ast_print_node(const csq_node *node, FILE *out, int indent) {
  if (!node) {
    print_indent(out, indent);
    fprintf(out, "(null)\n");
    return;
  }

  print_indent(out, indent);
  fprintf(out, "%s", node_type_to_string(node->type));

  if (node->type_info) {
    fprintf(out, " : ");
    ast_print_type(node->type_info, out);
  }

  fprintf(out, " @ %zu:%zu", node->line, node->column);
  fprintf(out, "\n");

  int child_indent = indent + 1;

  switch (node->type) {
  case NODE_PROGRAM:
    print_node_list(&node->data.program.statements, out, child_indent, "stmts");
    break;

  case NODE_BLOCK:
    print_node_list(&node->data.block.statements, out, child_indent, "stmts");
    break;

  case NODE_VAR_DECL:
    print_indent(out, child_indent);
    fprintf(out, "name:\n");
    ast_print_node(node->data.var_decl.name, out, child_indent + 1);
    if (node->data.var_decl.type_spec) {
      print_indent(out, child_indent);
      fprintf(out, "type:\n");
      ast_print_node(node->data.var_decl.type_spec, out, child_indent + 1);
    }
    if (node->data.var_decl.init) {
      print_indent(out, child_indent);
      fprintf(out, "init:\n");
      ast_print_node(node->data.var_decl.init, out, child_indent + 1);
    }
    break;

  case NODE_CONST_DECL:
    print_indent(out, child_indent);
    fprintf(out, "name:\n");
    ast_print_node(node->data.const_decl.name, out, child_indent + 1);
    if (node->data.const_decl.type_spec) {
      print_indent(out, child_indent);
      fprintf(out, "type:\n");
      ast_print_node(node->data.const_decl.type_spec, out, child_indent + 1);
    }
    print_indent(out, child_indent);
    fprintf(out, "init:\n");
    ast_print_node(node->data.const_decl.init, out, child_indent + 1);
    break;

  case NODE_FUNCTION_DECL:
    print_indent(out, child_indent);
    fprintf(out, "name:\n");
    ast_print_node(node->data.function_decl.name, out, child_indent + 1);
    print_node_list(&node->data.function_decl.params, out, child_indent,
                    "params");
    if (node->data.function_decl.return_type) {
      print_indent(out, child_indent);
      fprintf(out, "ret:\n");
      ast_print_node(node->data.function_decl.return_type, out,
                     child_indent + 1);
    }
    print_indent(out, child_indent);
    fprintf(out, "body:\n");
    ast_print_node(node->data.function_decl.body, out, child_indent + 1);
    break;

  case NODE_PARAM:
    print_indent(out, child_indent);
    fprintf(out, "name:\n");
    ast_print_node(node->data.param.name, out, child_indent + 1);
    if (node->data.param.type_spec) {
      print_indent(out, child_indent);
      fprintf(out, "type:\n");
      ast_print_node(node->data.param.type_spec, out, child_indent + 1);
    }
    if (node->data.param.default_val) {
      print_indent(out, child_indent);
      fprintf(out, "default:\n");
      ast_print_node(node->data.param.default_val, out, child_indent + 1);
    }
    break;

  case NODE_IF:
    print_indent(out, child_indent);
    fprintf(out, "cond:\n");
    ast_print_node(node->data.if_stmt.condition, out, child_indent + 1);
    print_indent(out, child_indent);
    fprintf(out, "then:\n");
    ast_print_node(node->data.if_stmt.then_branch, out, child_indent + 1);
    if (node->data.if_stmt.else_branch) {
      print_indent(out, child_indent);
      fprintf(out, "else:\n");
      ast_print_node(node->data.if_stmt.else_branch, out, child_indent + 1);
    }
    break;

  case NODE_WHILE:
    print_indent(out, child_indent);
    fprintf(out, "cond:\n");
    ast_print_node(node->data.while_stmt.condition, out, child_indent + 1);
    print_indent(out, child_indent);
    fprintf(out, "body:\n");
    ast_print_node(node->data.while_stmt.body, out, child_indent + 1);
    break;

  case NODE_FOR:
    print_indent(out, child_indent);
    fprintf(out, "var:\n");
    ast_print_node(node->data.for_stmt.var, out, child_indent + 1);
    print_indent(out, child_indent);
    fprintf(out, "iter:\n");
    ast_print_node(node->data.for_stmt.iterable, out, child_indent + 1);
    print_indent(out, child_indent);
    fprintf(out, "body:\n");
    ast_print_node(node->data.for_stmt.body, out, child_indent + 1);
    break;

  case NODE_RETURN:
    if (node->data.return_stmt.value) {
      print_indent(out, child_indent);
      fprintf(out, "value:\n");
      ast_print_node(node->data.return_stmt.value, out, child_indent + 1);
    }
    break;

  case NODE_THROW:
    print_indent(out, child_indent);
    fprintf(out, "value:\n");
    ast_print_node(node->data.throw_stmt.value, out, child_indent + 1);
    break;

  case NODE_BINARY_OP:
    print_indent(out, child_indent);
    fprintf(out, "op: %s\n", binary_op_to_string(node->data.binary.op));
    print_indent(out, child_indent);
    fprintf(out, "left:\n");
    ast_print_node(node->data.binary.left, out, child_indent + 1);
    print_indent(out, child_indent);
    fprintf(out, "right:\n");
    ast_print_node(node->data.binary.right, out, child_indent + 1);
    break;

  case NODE_UNARY_OP:
    print_indent(out, child_indent);
    fprintf(out, "op: %s\n", unary_op_to_string(node->data.unary.op));
    print_indent(out, child_indent);
    fprintf(out, "operand:\n");
    ast_print_node(node->data.unary.operand, out, child_indent + 1);
    break;

  case NODE_CALL:
    print_indent(out, child_indent);
    fprintf(out, "callee:\n");
    ast_print_node(node->data.call.callee, out, child_indent + 1);
    print_node_list(&node->data.call.args, out, child_indent, "args");
    break;

  case NODE_INDEX:
    print_indent(out, child_indent);
    fprintf(out, "object:\n");
    ast_print_node(node->data.index.object, out, child_indent + 1);
    print_indent(out, child_indent);
    fprintf(out, "index:\n");
    ast_print_node(node->data.index.index, out, child_indent + 1);
    break;

  case NODE_ACCESS:
    print_indent(out, child_indent);
    fprintf(out, "object:\n");
    ast_print_node(node->data.access.object, out, child_indent + 1);
    print_indent(out, child_indent);
    fprintf(out, "member:\n");
    ast_print_node(node->data.access.member, out, child_indent + 1);
    print_indent(out, child_indent);
    fprintf(out, "arrow: %s\n", node->data.access.is_arrow ? "true" : "false");
    break;

  case NODE_IDENTIFIER:
    print_indent(out, child_indent);
    fprintf(out, "name: %.*s\n", (int)node->data.identifier.len,
            node->data.identifier.name);
    break;

  case NODE_LITERAL_INT:
    print_indent(out, child_indent);
    fprintf(out, "value: %lld\n", node->data.literal_int.value);
    break;

  case NODE_LITERAL_FLOAT:
    print_indent(out, child_indent);
    fprintf(out, "value: %f\n", node->data.literal_float.value);
    break;

  case NODE_LITERAL_STRING:
    print_indent(out, child_indent);
    fprintf(out, "value: \"%.*s\"\n", (int)node->data.literal_string.len,
            node->data.literal_string.value);
    break;

  case NODE_LITERAL_BOOL:
    print_indent(out, child_indent);
    fprintf(out, "value: %s\n",
            node->data.literal_bool.value ? "true" : "false");
    break;

  case NODE_LITERAL_TAG:
    print_indent(out, child_indent);
    fprintf(out, "name: #%.*s\n", (int)node->data.literal_tag.len,
            node->data.literal_tag.name);
    break;

  case NODE_ARRAY_LITERAL:
    print_node_list(&node->data.array_literal.elements, out, child_indent,
                    "elems");
    break;

  case NODE_EXPR_STMT:
    print_indent(out, child_indent);
    fprintf(out, "expr:\n");
    ast_print_node(node->data.expr_stmt.expr, out, child_indent + 1);
    break;

  default:
    break;
  }
}

static void print_node_list(const node_list *list, FILE *out, int indent,
                            const char *label) {
  print_indent(out, indent);
  fprintf(out, "%s [%zu]:\n", label, list->count);
  for (size_t i = 0; i < list->count; i++) {
    ast_print_node(list->items[i], out, indent + 1);
  }
}

void ast_print_type(const csq_type *type, FILE *out) {
  if (!type) {
    fprintf(out, "unknown");
    return;
  }

  switch (type->kind) {
  case TYPE_VOID:
    fprintf(out, "void");
    break;
  case TYPE_BOOL:
    fprintf(out, "bool");
    break;
  case TYPE_INT:
    fprintf(out, "int");
    break;
  case TYPE_F32:
    fprintf(out, "f32");
    break;
  case TYPE_F64:
    fprintf(out, "f64");
    break;

  case TYPE_STRING:
    fprintf(out, "string");
    break;
  case TYPE_TAG:
    fprintf(out, "tag");
    break;
  case TYPE_POINTER:
    fprintf(out, "*");
    ast_print_type(type->data.pointer.base, out);
    break;
  case TYPE_ARRAY:
    fprintf(out, "[");
    ast_print_type(type->data.array.elem, out);
    fprintf(out, "; %zu]", type->data.array.count);
    break;
  case TYPE_SLICE:
    fprintf(out, "[]");
    ast_print_type(type->data.slice.elem, out);
    break;
  case TYPE_FUNCTION:
    fprintf(out, "fn(");
    for (size_t i = 0; i < type->data.function.params.count; i++) {
      if (i > 0)
        fprintf(out, ", ");
      ast_print_type(type->data.function.params.items[i], out);
    }
    fprintf(out, ")");
    if (type->data.function.ret->kind != TYPE_VOID) {
      fprintf(out, " -> ");
      ast_print_type(type->data.function.ret, out);
    }
    break;
  case TYPE_NAMED:
    fprintf(out, "%.*s", (int)type->data.named.name_len, type->data.named.name);
    break;
  default:
    fprintf(out, "<?>");
    break;
  }
}

void ast_print_symbol(const struct csq_symbol *sym, FILE *out) {
  if (!sym) {
    fprintf(out, "(null)");
    return;
  }

  const char *kind_str = "?";
  switch (sym->kind) {
  case SYM_VARIABLE:
    kind_str = "var";
    break;
  case SYM_CONSTANT:
    kind_str = "const";
    break;
  case SYM_FUNCTION:
    kind_str = "fn";
    break;
  case SYM_PARAMETER:
    kind_str = "param";
    break;
  case SYM_STRUCT:
    kind_str = "struct";
    break;
  case SYM_ENUM:
    kind_str = "enum";
    break;
  case SYM_FIELD:
    kind_str = "field";
    break;
  case SYM_TYPE_ALIAS:
    kind_str = "type";
    break;
  default:
    break;
  }

  fprintf(out, "%s %.*s", kind_str, (int)sym->name_len, sym->name);
  if (sym->type) {
    fprintf(out, ": ");
    ast_print_type(sym->type, out);
  }
}

void ast_print(const ast_context *ctx, FILE *out) {
  if (!ctx) {
    fprintf(out, "(null ast context)\n");
    return;
  }

  fprintf(out, "=== AST ===\n");
  if (ctx->root) {
    ast_print_node(ctx->root, out, 0);
  }
  fprintf(out, "===========\n");
}
