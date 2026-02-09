/**
 * @file ast.c
 * @brief Abstract Syntax Tree (AST) construction and management
 * @details Implements AST node creation, manipulation, and destruction for all
 * C² language constructs including declarations, statements, expressions,
 * and type specifications. Provides builder functions for constructing the
 * syntax tree.
 */

#include <parser/ast.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief Create a new AST node
 * @param type Node type enumeration
 * @param line Source line number
 * @param column Source column number
 * @return Newly allocated node, or NULL on allocation failure
 */
csq_node *node_create(node_type type, size_t line, size_t column) {
  csq_node *node = calloc(1, sizeof(csq_node));
  if (!node)
    return NULL;

  node->type = type;
  node->line = line;
  node->column = column;
  node->type_info = NULL;
  node->symbol = NULL;

  return node;
}

/**
 * @brief Recursively free AST node and its children
 * @details Deallocates node and all sub-nodes, handling cleanup based on node
 * type.
 * @param node Node to deallocate
 */
void node_free(csq_node *node) {
  if (!node)
    return;

  switch (node->type) {
  case NODE_PROGRAM:
    node_list_free(&node->data.program.statements);
    break;
  case NODE_BLOCK:
    node_list_free(&node->data.block.statements);
    break;
  case NODE_VAR_DECL:
  case NODE_CONST_DECL:
    if (node->data.var_decl.name)
      node_free(node->data.var_decl.name);
    if (node->data.var_decl.type_spec)
      node_free(node->data.var_decl.type_spec);
    if (node->data.var_decl.init)
      node_free(node->data.var_decl.init);
    break;
  case NODE_FUNCTION_DECL:
    if (node->data.function_decl.name)
      node_free(node->data.function_decl.name);
    node_list_free(&node->data.function_decl.params);
    if (node->data.function_decl.return_type)
      node_free(node->data.function_decl.return_type);
    if (node->data.function_decl.body)
      node_free(node->data.function_decl.body);
    node_list_free(&node->data.function_decl.generics);
    break;
  case NODE_PARAM:
    if (node->data.param.name)
      node_free(node->data.param.name);
    if (node->data.param.type_spec)
      node_free(node->data.param.type_spec);
    if (node->data.param.default_val)
      node_free(node->data.param.default_val);
    break;
  case NODE_STRUCT_DECL:
    if (node->data.struct_decl.name)
      node_free(node->data.struct_decl.name);
    node_list_free(&node->data.struct_decl.fields);
    node_list_free(&node->data.struct_decl.methods);
    node_list_free(&node->data.struct_decl.generics);
    break;
  case NODE_ENUM_DECL:
    if (node->data.enum_decl.name)
      node_free(node->data.enum_decl.name);
    node_list_free(&node->data.enum_decl.variants);
    break;
  case NODE_ENUM_VARIANT:
    if (node->data.enum_variant.name)
      node_free(node->data.enum_variant.name);
    node_list_free(&node->data.enum_variant.fields);
    break;
  case NODE_FIELD:
    if (node->data.field.name)
      node_free(node->data.field.name);
    if (node->data.field.type_spec)
      node_free(node->data.field.type_spec);
    break;
  case NODE_IF:
    if (node->data.if_stmt.condition)
      node_free(node->data.if_stmt.condition);
    if (node->data.if_stmt.then_branch)
      node_free(node->data.if_stmt.then_branch);
    if (node->data.if_stmt.else_branch)
      node_free(node->data.if_stmt.else_branch);
    break;
  case NODE_SWITCH:
    if (node->data.switch_stmt.expr)
      node_free(node->data.switch_stmt.expr);
    node_list_free(&node->data.switch_stmt.cases);
    if (node->data.switch_stmt.default_case)
      node_free(node->data.switch_stmt.default_case);
    break;
  case NODE_CASE:
    if (node->data.case_stmt.value)
      node_free(node->data.case_stmt.value);
    if (node->data.case_stmt.body)
      node_free(node->data.case_stmt.body);
    break;
  case NODE_WHILE:
    if (node->data.while_stmt.condition)
      node_free(node->data.while_stmt.condition);
    if (node->data.while_stmt.body)
      node_free(node->data.while_stmt.body);
    break;
  case NODE_FOR:
    if (node->data.for_stmt.var)
      node_free(node->data.for_stmt.var);
    if (node->data.for_stmt.iterable)
      node_free(node->data.for_stmt.iterable);
    if (node->data.for_stmt.body)
      node_free(node->data.for_stmt.body);
    break;
  case NODE_REPEAT:
    if (node->data.repeat_stmt.body)
      node_free(node->data.repeat_stmt.body);
    if (node->data.repeat_stmt.condition)
      node_free(node->data.repeat_stmt.condition);
    break;
  case NODE_RETURN:
    if (node->data.return_stmt.value)
      node_free(node->data.return_stmt.value);
    break;
  case NODE_THROW:
    if (node->data.throw_stmt.value)
      node_free(node->data.throw_stmt.value);
    break;
  case NODE_TRY:
    if (node->data.try_stmt.try_block)
      node_free(node->data.try_stmt.try_block);
    node_list_free(&node->data.try_stmt.catch_blocks);
    break;
  case NODE_CATCH:
    if (node->data.catch_block.var)
      node_free(node->data.catch_block.var);
    if (node->data.catch_block.type_spec)
      node_free(node->data.catch_block.type_spec);
    if (node->data.catch_block.block)
      node_free(node->data.catch_block.block);
    break;
  case NODE_DEFER:
    if (node->data.defer_stmt.stmt)
      node_free(node->data.defer_stmt.stmt);
    break;
  case NODE_BINARY_OP:
    if (node->data.binary.left)
      node_free(node->data.binary.left);
    if (node->data.binary.right)
      node_free(node->data.binary.right);
    break;
  case NODE_UNARY_OP:
    if (node->data.unary.operand)
      node_free(node->data.unary.operand);
    break;
  case NODE_CALL:
    if (node->data.call.callee)
      node_free(node->data.call.callee);
    node_list_free(&node->data.call.args);
    break;
  case NODE_INDEX:
    if (node->data.index.object)
      node_free(node->data.index.object);
    if (node->data.index.index)
      node_free(node->data.index.index);
    break;
  case NODE_ACCESS:
    if (node->data.access.object)
      node_free(node->data.access.object);
    if (node->data.access.member)
      node_free(node->data.access.member);
    break;
  case NODE_IDENTIFIER:
    free((void *)node->data.identifier.name);
    break;
  case NODE_LITERAL_STRING:
    free((void *)node->data.literal_string.value);
    break;
  case NODE_LITERAL_TAG:
    free((void *)node->data.literal_tag.name);
    break;
  case NODE_ARRAY_LITERAL:
    node_list_free(&node->data.array_literal.elements);
    break;
  case NODE_MAP_LITERAL:
    node_list_free(&node->data.map_literal.entries);
    break;
  case NODE_MAP_ENTRY:
    if (node->data.map_entry.key)
      node_free(node->data.map_entry.key);
    if (node->data.map_entry.value)
      node_free(node->data.map_entry.value);
    break;
  case NODE_SPAWN:
    if (node->data.spawn.call_expr)
      node_free(node->data.spawn.call_expr);
    break;
  case NODE_IMPORT:
    free((void *)node->data.import.path);
    if (node->data.import.alias)
      node_free(node->data.import.alias);
    break;
  case NODE_CAST:
    if (node->data.cast.expr)
      node_free(node->data.cast.expr);
    if (node->data.cast.target_type)
      node_free(node->data.cast.target_type);
    break;
  case NODE_TYPE_SPEC:
    if (node->data.type_spec.base)
      node_free(node->data.type_spec.base);
    node_list_free(&node->data.type_spec.args);
    break;
  case NODE_GENERIC_INST:
    if (node->data.generic_inst.base)
      node_free(node->data.generic_inst.base);
    node_list_free(&node->data.generic_inst.type_args);
    break;
  case NODE_RANGE:
    if (node->data.range.start)
      node_free(node->data.range.start);
    if (node->data.range.end)
      node_free(node->data.range.end);
    break;
  case NODE_TUPLE:
    node_list_free(&node->data.tuple.elements);
    break;
  case NODE_EXPR_STMT:
    if (node->data.expr_stmt.expr)
      node_free(node->data.expr_stmt.expr);
    break;
  default:
    break;
  }

  free(node);
}

/**
 * @brief Initialize a node list
 * @param list Node list structure to initialize
 */
void node_list_init(node_list *list) {
  list->items = NULL;
  list->count = 0;
  list->capacity = 0;
}

/**
 * @brief Free all nodes in a list
 * @param list Node list to deallocate
 */
void node_list_free(node_list *list) {
  if (!list)
    return;

  for (size_t i = 0; i < list->count; i++) {
    node_free(list->items[i]);
  }
  free(list->items);
  list->items = NULL;
  list->count = 0;
  list->capacity = 0;
}

/**
 * @brief Add node to node list
 * @details Resizes the list if necessary to accommodate the new node.
 * @param list Node list to add to
 * @param node Node to add
 */
void node_list_add(node_list *list, csq_node *node) {
  if (!list || !node)
    return;

  if (list->count >= list->capacity) {
    size_t new_capacity = list->capacity == 0 ? 8 : list->capacity * 2;
    csq_node **new_items =
        realloc(list->items, new_capacity * sizeof(csq_node *));
    if (!new_items)
      return;

    list->items = new_items;
    list->capacity = new_capacity;
  }

  list->items[list->count++] = node;
}

/**
 * @brief Create AST context structure
 * @details Initializes the context that holds the root AST node and symbol
 * table.
 * @return Allocated context, or NULL on failure
 */
ast_context *ast_context_create(void) {
  ast_context *ctx = malloc(sizeof(ast_context));
  if (!ctx)
    return NULL;

  ctx->root = NULL;
  ctx->symbols = sym_table_create();
  ctx->has_errors = false;

  return ctx;
}

/**
 * @brief Free AST context and all nodes
 * @param ctx Context to deallocate
 */
void ast_context_free(ast_context *ctx) {
  if (!ctx)
    return;

  if (ctx->root)
    node_free(ctx->root);
  if (ctx->symbols)
    sym_table_free(ctx->symbols);

  free(ctx);
}

csq_node *ast_create_program(void) {
  csq_node *node = node_create(NODE_PROGRAM, 0, 0);
  if (node)
    node_list_init(&node->data.program.statements);
  return node;
}

csq_node *ast_create_block(void) {
  csq_node *node = node_create(NODE_BLOCK, 0, 0);
  if (node)
    node_list_init(&node->data.block.statements);
  return node;
}

csq_node *ast_create_var_decl(csq_node *name, csq_node *type_spec,
                              csq_node *init, bool is_mutable) {
  csq_node *node = node_create(NODE_VAR_DECL, name ? name->line : 0,
                               name ? name->column : 0);
  if (!node)
    return NULL;

  node->data.var_decl.name = name;
  node->data.var_decl.type_spec = type_spec;
  node->data.var_decl.init = init;
  node->data.var_decl.is_mutable = is_mutable;

  return node;
}

csq_node *ast_create_const_decl(csq_node *name, csq_node *type_spec,
                                csq_node *init) {
  csq_node *node = node_create(NODE_CONST_DECL, name ? name->line : 0,
                               name ? name->column : 0);
  if (!node)
    return NULL;

  node->data.const_decl.name = name;
  node->data.const_decl.type_spec = type_spec;
  node->data.const_decl.init = init;

  return node;
}

csq_node *ast_create_function_decl(csq_node *name, node_list *params,
                                   csq_node *ret_type, csq_node *body) {
  csq_node *node = node_create(NODE_FUNCTION_DECL, name ? name->line : 0,
                               name ? name->column : 0);
  if (!node)
    return NULL;

  node->data.function_decl.name = name;
  node_list_init(&node->data.function_decl.params);
  if (params) {
    for (size_t i = 0; i < params->count; i++) {
      node_list_add(&node->data.function_decl.params, params->items[i]);
    }
  }
  node->data.function_decl.return_type = ret_type;
  node->data.function_decl.body = body;
  node_list_init(&node->data.function_decl.generics);
  node->data.function_decl.is_async = false;

  return node;
}

csq_node *ast_create_param(csq_node *name, csq_node *type_spec,
                           csq_node *default_val) {
  csq_node *node =
      node_create(NODE_PARAM, name ? name->line : 0, name ? name->column : 0);
  if (!node)
    return NULL;

  node->data.param.name = name;
  node->data.param.type_spec = type_spec;
  node->data.param.default_val = default_val;

  return node;
}

csq_node *ast_create_struct_decl(csq_node *name, node_list *fields) {
  csq_node *node = node_create(NODE_STRUCT_DECL, name ? name->line : 0,
                               name ? name->column : 0);
  if (!node)
    return NULL;

  node->data.struct_decl.name = name;
  node_list_init(&node->data.struct_decl.fields);
  if (fields) {
    for (size_t i = 0; i < fields->count; i++) {
      node_list_add(&node->data.struct_decl.fields, fields->items[i]);
    }
  }
  node_list_init(&node->data.struct_decl.methods);
  node_list_init(&node->data.struct_decl.generics);

  return node;
}

csq_node *ast_create_enum_decl(csq_node *name, node_list *variants) {
  csq_node *node = node_create(NODE_ENUM_DECL, name ? name->line : 0,
                               name ? name->column : 0);
  if (!node)
    return NULL;

  node->data.enum_decl.name = name;
  node_list_init(&node->data.enum_decl.variants);
  if (variants) {
    for (size_t i = 0; i < variants->count; i++) {
      node_list_add(&node->data.enum_decl.variants, variants->items[i]);
    }
  }

  return node;
}

csq_node *ast_create_enum_variant(csq_node *name, node_list *fields,
                                  int value) {
  csq_node *node = node_create(NODE_ENUM_VARIANT, name ? name->line : 0,
                               name ? name->column : 0);
  if (!node)
    return NULL;

  node->data.enum_variant.name = name;
  node_list_init(&node->data.enum_variant.fields);
  if (fields) {
    for (size_t i = 0; i < fields->count; i++) {
      node_list_add(&node->data.enum_variant.fields, fields->items[i]);
    }
  }
  node->data.enum_variant.value = value;

  return node;
}

csq_node *ast_create_field(csq_node *name, csq_node *type_spec) {
  csq_node *node =
      node_create(NODE_FIELD, name ? name->line : 0, name ? name->column : 0);
  if (!node)
    return NULL;

  node->data.field.name = name;
  node->data.field.type_spec = type_spec;

  return node;
}

csq_node *ast_create_if(csq_node *condition, csq_node *then_branch,
                        csq_node *else_branch) {
  csq_node *node = node_create(NODE_IF, condition ? condition->line : 0,
                               condition ? condition->column : 0);
  if (!node)
    return NULL;

  node->data.if_stmt.condition = condition;
  node->data.if_stmt.then_branch = then_branch;
  node->data.if_stmt.else_branch = else_branch;

  return node;
}

csq_node *ast_create_switch(csq_node *expr, node_list *cases,
                            csq_node *default_case) {
  csq_node *node =
      node_create(NODE_SWITCH, expr ? expr->line : 0, expr ? expr->column : 0);
  if (!node)
    return NULL;

  node->data.switch_stmt.expr = expr;
  node_list_init(&node->data.switch_stmt.cases);
  if (cases) {
    for (size_t i = 0; i < cases->count; i++) {
      node_list_add(&node->data.switch_stmt.cases, cases->items[i]);
    }
  }
  node->data.switch_stmt.default_case = default_case;

  return node;
}

csq_node *ast_create_case(csq_node *value, csq_node *body) {
  csq_node *node = node_create(NODE_CASE, value ? value->line : 0,
                               value ? value->column : 0);
  if (!node)
    return NULL;

  node->data.case_stmt.value = value;
  node->data.case_stmt.body = body;

  return node;
}

csq_node *ast_create_while(csq_node *condition, csq_node *body) {
  csq_node *node = node_create(NODE_WHILE, condition ? condition->line : 0,
                               condition ? condition->column : 0);
  if (!node)
    return NULL;

  node->data.while_stmt.condition = condition;
  node->data.while_stmt.body = body;

  return node;
}

csq_node *ast_create_for(csq_node *var, csq_node *iterable, csq_node *body) {
  csq_node *node =
      node_create(NODE_FOR, var ? var->line : 0, var ? var->column : 0);
  if (!node)
    return NULL;

  node->data.for_stmt.var = var;
  node->data.for_stmt.iterable = iterable;
  node->data.for_stmt.body = body;

  return node;
}

csq_node *ast_create_repeat(csq_node *body, csq_node *condition) {
  csq_node *node =
      node_create(NODE_REPEAT, body ? body->line : 0, body ? body->column : 0);
  if (!node)
    return NULL;

  node->data.repeat_stmt.body = body;
  node->data.repeat_stmt.condition = condition;

  return node;
}

csq_node *ast_create_return(csq_node *value) {
  csq_node *node = node_create(NODE_RETURN, value ? value->line : 0,
                               value ? value->column : 0);
  if (!node)
    return NULL;

  node->data.return_stmt.value = value;

  return node;
}

csq_node *ast_create_throw(csq_node *value) {
  csq_node *node = node_create(NODE_THROW, value ? value->line : 0,
                               value ? value->column : 0);
  if (!node)
    return NULL;

  node->data.throw_stmt.value = value;

  return node;
}

csq_node *ast_create_try(csq_node *try_block, node_list *catches) {
  csq_node *node = node_create(NODE_TRY, try_block ? try_block->line : 0,
                               try_block ? try_block->column : 0);
  if (!node)
    return NULL;

  node->data.try_stmt.try_block = try_block;
  node_list_init(&node->data.try_stmt.catch_blocks);
  if (catches) {
    for (size_t i = 0; i < catches->count; i++) {
      node_list_add(&node->data.try_stmt.catch_blocks, catches->items[i]);
    }
  }

  return node;
}

csq_node *ast_create_catch(csq_node *var, csq_node *type_spec,
                           csq_node *block) {
  csq_node *node =
      node_create(NODE_CATCH, var ? var->line : 0, var ? var->column : 0);
  if (!node)
    return NULL;

  node->data.catch_block.var = var;
  node->data.catch_block.type_spec = type_spec;
  node->data.catch_block.block = block;

  return node;
}

csq_node *ast_create_defer(csq_node *stmt) {
  csq_node *node =
      node_create(NODE_DEFER, stmt ? stmt->line : 0, stmt ? stmt->column : 0);
  if (!node)
    return NULL;

  node->data.defer_stmt.stmt = stmt;

  return node;
}

csq_node *ast_create_break(void) { return node_create(NODE_BREAK, 0, 0); }

csq_node *ast_create_continue(void) { return node_create(NODE_CONTINUE, 0, 0); }

csq_node *ast_create_binary(binary_op op, csq_node *left, csq_node *right) {
  csq_node *node = node_create(NODE_BINARY_OP, left ? left->line : 0,
                               left ? left->column : 0);
  if (!node)
    return NULL;

  node->data.binary.op = op;
  node->data.binary.left = left;
  node->data.binary.right = right;

  return node;
}

csq_node *ast_create_unary(unary_op op, csq_node *operand) {
  csq_node *node = node_create(NODE_UNARY_OP, operand ? operand->line : 0,
                               operand ? operand->column : 0);
  if (!node)
    return NULL;

  node->data.unary.op = op;
  node->data.unary.operand = operand;

  return node;
}

csq_node *ast_create_call(csq_node *callee, node_list *args) {
  csq_node *node = node_create(NODE_CALL, callee ? callee->line : 0,
                               callee ? callee->column : 0);
  if (!node)
    return NULL;

  node->data.call.callee = callee;
  node_list_init(&node->data.call.args);
  if (args) {
    for (size_t i = 0; i < args->count; i++) {
      node_list_add(&node->data.call.args, args->items[i]);
    }
  }

  return node;
}

csq_node *ast_create_index(csq_node *object, csq_node *index) {
  csq_node *node = node_create(NODE_INDEX, object ? object->line : 0,
                               object ? object->column : 0);
  if (!node)
    return NULL;

  node->data.index.object = object;
  node->data.index.index = index;

  return node;
}

csq_node *ast_create_access(csq_node *object, csq_node *member, bool is_arrow) {
  csq_node *node = node_create(NODE_ACCESS, object ? object->line : 0,
                               object ? object->column : 0);
  if (!node)
    return NULL;

  node->data.access.object = object;
  node->data.access.member = member;
  node->data.access.is_arrow = is_arrow;

  return node;
}

csq_node *ast_create_identifier(const char *name, size_t len) {
  csq_node *node = node_create(NODE_IDENTIFIER, 0, 0);
  if (!node)
    return NULL;

  char *name_copy = malloc(len + 1);
  if (!name_copy) {
    node_free(node);
    return NULL;
  }
  memcpy(name_copy, name, len);
  name_copy[len] = '\0';

  node->data.identifier.name = name_copy;
  node->data.identifier.len = len;

  return node;
}

csq_node *ast_create_literal_int(long long value) {
  csq_node *node = node_create(NODE_LITERAL_INT, 0, 0);
  if (node)
    node->data.literal_int.value = value;
  return node;
}

csq_node *ast_create_literal_float(double value) {
  csq_node *node = node_create(NODE_LITERAL_FLOAT, 0, 0);
  if (node)
    node->data.literal_float.value = value;
  return node;
}

csq_node *ast_create_literal_string(const char *value, size_t len) {
  csq_node *node = node_create(NODE_LITERAL_STRING, 0, 0);
  if (!node)
    return NULL;

  char *str = malloc(len + 1);
  if (!str) {
    node_free(node);
    return NULL;
  }
  memcpy(str, value, len);
  str[len] = '\0';

  node->data.literal_string.value = str;
  node->data.literal_string.len = len;

  return node;
}

csq_node *ast_create_literal_bool(bool value) {
  csq_node *node = node_create(NODE_LITERAL_BOOL, 0, 0);
  if (node)
    node->data.literal_bool.value = value;
  return node;
}

csq_node *ast_create_literal_nil(void) {
  return node_create(NODE_LITERAL_NIL, 0, 0);
}

csq_node *ast_create_literal_tag(const char *name, size_t len) {
  csq_node *node = node_create(NODE_LITERAL_TAG, 0, 0);
  if (!node)
    return NULL;

  char *name_copy = malloc(len + 1);
  if (!name_copy) {
    node_free(node);
    return NULL;
  }
  memcpy(name_copy, name, len);
  name_copy[len] = '\0';

  node->data.literal_tag.name = name_copy;
  node->data.literal_tag.len = len;

  return node;
}

csq_node *ast_create_array_literal(node_list *elements) {
  csq_node *node = node_create(NODE_ARRAY_LITERAL, 0, 0);
  if (!node)
    return NULL;

  node_list_init(&node->data.array_literal.elements);
  if (elements) {
    for (size_t i = 0; i < elements->count; i++) {
      node_list_add(&node->data.array_literal.elements, elements->items[i]);
    }
  }

  return node;
}

csq_node *ast_create_map_literal(node_list *entries) {
  csq_node *node = node_create(NODE_MAP_LITERAL, 0, 0);
  if (!node)
    return NULL;

  node_list_init(&node->data.map_literal.entries);
  if (entries) {
    for (size_t i = 0; i < entries->count; i++) {
      node_list_add(&node->data.map_literal.entries, entries->items[i]);
    }
  }

  return node;
}

csq_node *ast_create_map_entry(csq_node *key, csq_node *value) {
  csq_node *node =
      node_create(NODE_MAP_ENTRY, key ? key->line : 0, key ? key->column : 0);
  if (!node)
    return NULL;

  node->data.map_entry.key = key;
  node->data.map_entry.value = value;

  return node;
}

csq_node *ast_create_spawn(csq_node *call_expr) {
  csq_node *node = node_create(NODE_SPAWN, call_expr ? call_expr->line : 0,
                               call_expr ? call_expr->column : 0);
  if (!node)
    return NULL;

  node->data.spawn.call_expr = call_expr;

  return node;
}

csq_node *ast_create_import(const char *path, size_t len, csq_node *alias,
                            bool is_wildcard) {
  csq_node *node = node_create(NODE_IMPORT, 0, 0);
  if (!node)
    return NULL;

  char *path_copy = malloc(len + 1);
  if (!path_copy) {
    node_free(node);
    return NULL;
  }
  memcpy(path_copy, path, len);
  path_copy[len] = '\0';

  node->data.import.path = path_copy;
  node->data.import.len = len;
  node->data.import.alias = alias;
  node->data.import.is_wildcard = is_wildcard;

  return node;
}

csq_node *ast_create_self(void) { return node_create(NODE_SELF, 0, 0); }

csq_node *ast_create_cast(csq_node *expr, csq_node *target_type) {
  csq_node *node =
      node_create(NODE_CAST, expr ? expr->line : 0, expr ? expr->column : 0);
  if (!node)
    return NULL;

  node->data.cast.expr = expr;
  node->data.cast.target_type = target_type;

  return node;
}

csq_node *ast_create_type_spec(csq_node *base, node_list *args) {
  csq_node *node = node_create(NODE_TYPE_SPEC, base ? base->line : 0,
                               base ? base->column : 0);
  if (!node)
    return NULL;

  node->data.type_spec.base = base;
  node_list_init(&node->data.type_spec.args);
  if (args) {
    for (size_t i = 0; i < args->count; i++) {
      node_list_add(&node->data.type_spec.args, args->items[i]);
    }
  }

  return node;
}

csq_node *ast_create_generic_inst(csq_node *base, node_list *type_args) {
  csq_node *node = node_create(NODE_GENERIC_INST, base ? base->line : 0,
                               base ? base->column : 0);
  if (!node)
    return NULL;

  node->data.generic_inst.base = base;
  node_list_init(&node->data.generic_inst.type_args);
  if (type_args) {
    for (size_t i = 0; i < type_args->count; i++) {
      node_list_add(&node->data.generic_inst.type_args, type_args->items[i]);
    }
  }

  return node;
}

csq_node *ast_create_range(csq_node *start, csq_node *end, bool inclusive) {
  csq_node *node = node_create(NODE_RANGE, start ? start->line : 0,
                               start ? start->column : 0);
  if (!node)
    return NULL;

  node->data.range.start = start;
  node->data.range.end = end;
  node->data.range.inclusive = inclusive;

  return node;
}

csq_node *ast_create_tuple(node_list *elements) {
  csq_node *node = node_create(NODE_TUPLE, 0, 0);
  if (!node)
    return NULL;

  node_list_init(&node->data.tuple.elements);
  if (elements) {
    for (size_t i = 0; i < elements->count; i++) {
      node_list_add(&node->data.tuple.elements, elements->items[i]);
    }
  }

  return node;
}

csq_node *ast_create_expr_stmt(csq_node *expr) {
  csq_node *node = node_create(NODE_EXPR_STMT, expr ? expr->line : 0,
                               expr ? expr->column : 0);
  if (!node)
    return NULL;

  node->data.expr_stmt.expr = expr;

  return node;
}

csq_node *ast_create_empty(void) { return node_create(NODE_EMPTY, 0, 0); }

/**
 * @brief Convert node type to string representation
 * @param type Node type enumeration
 * @return String name of the node type
 */
const char *node_type_to_string(node_type type) {
  switch (type) {
  case NODE_PROGRAM:
    return "program";
  case NODE_BLOCK:
    return "block";
  case NODE_VAR_DECL:
    return "var_decl";
  case NODE_CONST_DECL:
    return "const_decl";
  case NODE_FUNCTION_DECL:
    return "function_decl";
  case NODE_PARAM:
    return "param";
  case NODE_STRUCT_DECL:
    return "struct_decl";
  case NODE_ENUM_DECL:
    return "enum_decl";
  case NODE_ENUM_VARIANT:
    return "enum_variant";
  case NODE_FIELD:
    return "field";
  case NODE_IF:
    return "if";
  case NODE_SWITCH:
    return "switch";
  case NODE_CASE:
    return "case";
  case NODE_WHILE:
    return "while";
  case NODE_FOR:
    return "for";
  case NODE_REPEAT:
    return "repeat";
  case NODE_RETURN:
    return "return";
  case NODE_THROW:
    return "throw";
  case NODE_TRY:
    return "try";
  case NODE_CATCH:
    return "catch";
  case NODE_DEFER:
    return "defer";
  case NODE_BREAK:
    return "break";
  case NODE_CONTINUE:
    return "continue";
  case NODE_ASSIGN:
    return "assign";
  case NODE_BINARY_OP:
    return "binary_op";
  case NODE_UNARY_OP:
    return "unary_op";
  case NODE_CALL:
    return "call";
  case NODE_INDEX:
    return "index";
  case NODE_ACCESS:
    return "access";
  case NODE_IDENTIFIER:
    return "identifier";
  case NODE_LITERAL_INT:
    return "literal_int";
  case NODE_LITERAL_FLOAT:
    return "literal_float";
  case NODE_LITERAL_STRING:
    return "literal_string";
  case NODE_LITERAL_BOOL:
    return "literal_bool";
  case NODE_LITERAL_NIL:
    return "literal_nil";
  case NODE_LITERAL_TAG:
    return "literal_tag";
  case NODE_ARRAY_LITERAL:
    return "array_literal";
  case NODE_MAP_LITERAL:
    return "map_literal";
  case NODE_MAP_ENTRY:
    return "map_entry";
  case NODE_SPAWN:
    return "spawn";
  case NODE_IMPORT:
    return "import";
  case NODE_SELF:
    return "self";
  case NODE_CAST:
    return "cast";
  case NODE_TYPE_SPEC:
    return "type_spec";
  case NODE_GENERIC_INST:
    return "generic_inst";
  case NODE_RANGE:
    return "range";
  case NODE_TUPLE:
    return "tuple";
  case NODE_EXPR_STMT:
    return "expr_stmt";
  case NODE_EMPTY:
    return "empty";
  default:
    return "unknown";
  }
}

/**
 * @brief Convert binary operator to string
 * @param op Binary operator enumeration
 * @return String representation of the operator
 */
const char *binary_op_to_string(binary_op op) {
  switch (op) {
  case BINOP_ADD:
    return "+";
  case BINOP_SUB:
    return "-";
  case BINOP_MUL:
    return "*";
  case BINOP_DIV:
    return "/";
  case BINOP_MOD:
    return "%";
  case BINOP_POW:
    return "^";
  case BINOP_EQ:
    return "==";
  case BINOP_NE:
    return "!=";
  case BINOP_LT:
    return "<";
  case BINOP_GT:
    return ">";
  case BINOP_LE:
    return "<=";
  case BINOP_GE:
    return ">=";
  case BINOP_AND:
    return "and";
  case BINOP_OR:
    return "or";
  case BINOP_BIT_AND:
    return "&";
  case BINOP_BIT_OR:
    return "|";
  case BINOP_BIT_XOR:
    return "^";
  case BINOP_SHL:
    return "<<";
  case BINOP_SHR:
    return ">>";
  case BINOP_ASSIGN:
    return "=";
  case BINOP_ADD_ASSIGN:
    return "+=";
  case BINOP_SUB_ASSIGN:
    return "-=";
  case BINOP_MUL_ASSIGN:
    return "*=";
  case BINOP_DIV_ASSIGN:
    return "/=";
  case BINOP_MOD_ASSIGN:
    return "%=";
  default:
    return "?";
  }
}

/**
 * @brief Convert unary operator to string
 * @param op Unary operator enumeration
 * @return String representation of the operator
 */
const char *unary_op_to_string(unary_op op) {
  switch (op) {
  case UNOP_NEG:
    return "-";
  case UNOP_NOT:
    return "not";
  case UNOP_BIT_NOT:
    return "~";
  case UNOP_PRE_INC:
    return "++";
  case UNOP_PRE_DEC:
    return "--";
  case UNOP_POST_INC:
    return "++";
  case UNOP_POST_DEC:
    return "--";
  case UNOP_DEREF:
    return "*";
  case UNOP_ADDR:
    return "&";
  default:
    return "?";
  }
}
