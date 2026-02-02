#ifndef PARSER_AST_H
#define PARSER_AST_H

#include <parser/node.h>
#include <parser/symbtab.h>
#include <parser/types.h>

typedef struct {
  rvn_node *root;
  sym_table *symbols;
  bool has_errors;
} ast_context;

ast_context *ast_context_create(void);
void ast_context_free(ast_context *ctx);

rvn_node *ast_create_program(void);
rvn_node *ast_create_block(void);
rvn_node *ast_create_var_decl(rvn_node *name, rvn_node *type_spec,
                              rvn_node *init, bool is_mutable);
rvn_node *ast_create_const_decl(rvn_node *name, rvn_node *type_spec,
                                rvn_node *init);
rvn_node *ast_create_function_decl(rvn_node *name, node_list *params,
                                   rvn_node *ret_type, rvn_node *body);
rvn_node *ast_create_param(rvn_node *name, rvn_node *type_spec,
                           rvn_node *default_val);
rvn_node *ast_create_struct_decl(rvn_node *name, node_list *fields);
rvn_node *ast_create_enum_decl(rvn_node *name, node_list *variants);
rvn_node *ast_create_enum_variant(rvn_node *name, node_list *fields, int value);
rvn_node *ast_create_field(rvn_node *name, rvn_node *type_spec);
rvn_node *ast_create_if(rvn_node *condition, rvn_node *then_branch,
                        rvn_node *else_branch);
rvn_node *ast_create_switch(rvn_node *expr, node_list *cases,
                            rvn_node *default_case);
rvn_node *ast_create_case(rvn_node *value, rvn_node *body);
rvn_node *ast_create_while(rvn_node *condition, rvn_node *body);
rvn_node *ast_create_for(rvn_node *var, rvn_node *iterable, rvn_node *body);
rvn_node *ast_create_repeat(rvn_node *body, rvn_node *condition);
rvn_node *ast_create_return(rvn_node *value);
rvn_node *ast_create_throw(rvn_node *value);
rvn_node *ast_create_try(rvn_node *try_block, node_list *catches);
rvn_node *ast_create_catch(rvn_node *var, rvn_node *type_spec, rvn_node *block);
rvn_node *ast_create_defer(rvn_node *stmt);
rvn_node *ast_create_break(void);
rvn_node *ast_create_continue(void);
rvn_node *ast_create_binary(binary_op op, rvn_node *left, rvn_node *right);
rvn_node *ast_create_unary(unary_op op, rvn_node *operand);
rvn_node *ast_create_call(rvn_node *callee, node_list *args);
rvn_node *ast_create_index(rvn_node *object, rvn_node *index);
rvn_node *ast_create_access(rvn_node *object, rvn_node *member, bool is_arrow);
rvn_node *ast_create_identifier(const char *name, size_t len);
rvn_node *ast_create_literal_int(long long value);
rvn_node *ast_create_literal_float(double value);
rvn_node *ast_create_literal_string(const char *value, size_t len);
rvn_node *ast_create_literal_bool(bool value);
rvn_node *ast_create_literal_null(void);
rvn_node *ast_create_literal_tag(const char *name, size_t len);
rvn_node *ast_create_array_literal(node_list *elements);
rvn_node *ast_create_map_literal(node_list *entries);
rvn_node *ast_create_map_entry(rvn_node *key, rvn_node *value);
rvn_node *ast_create_spawn(rvn_node *call_expr);
rvn_node *ast_create_import(const char *path, size_t len, rvn_node *alias,
                            bool is_wildcard);
rvn_node *ast_create_self(void);
rvn_node *ast_create_cast(rvn_node *expr, rvn_node *target_type);
rvn_node *ast_create_type_spec(rvn_node *base, node_list *args);
rvn_node *ast_create_generic_inst(rvn_node *base, node_list *type_args);
rvn_node *ast_create_range(rvn_node *start, rvn_node *end, bool inclusive);
rvn_node *ast_create_tuple(node_list *elements);
rvn_node *ast_create_expr_stmt(rvn_node *expr);
rvn_node *ast_create_empty(void);

const char *node_type_to_string(node_type type);
const char *binary_op_to_string(binary_op op);
const char *unary_op_to_string(unary_op op);

#endif
