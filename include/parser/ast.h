#ifndef PARSER_AST_H
#define PARSER_AST_H

#include <parser/node.h>
#include <parser/symbtab.h>
#include <parser/types.h>

typedef struct {
  csq_node *root;
  sym_table *symbols;
  bool has_errors;
} ast_context;

ast_context *ast_context_create(void);
void ast_context_free(ast_context *ctx);

csq_node *ast_create_program(void);
csq_node *ast_create_block(void);
csq_node *ast_create_var_decl(csq_node *name, csq_node *type_spec,
                              csq_node *init, bool is_mutable);
csq_node *ast_create_const_decl(csq_node *name, csq_node *type_spec,
                                csq_node *init);
csq_node *ast_create_function_decl(csq_node *name, node_list *params,
                                   csq_node *ret_type, csq_node *body);
csq_node *ast_create_param(csq_node *name, csq_node *type_spec,
                           csq_node *default_val);
csq_node *ast_create_struct_decl(csq_node *name, node_list *fields);
csq_node *ast_create_enum_decl(csq_node *name, node_list *variants);
csq_node *ast_create_enum_variant(csq_node *name, node_list *fields, int value);
csq_node *ast_create_field(csq_node *name, csq_node *type_spec);
csq_node *ast_create_if(csq_node *condition, csq_node *then_branch,
                        csq_node *else_branch);
csq_node *ast_create_switch(csq_node *expr, node_list *cases,
                            csq_node *default_case);
csq_node *ast_create_case(csq_node *value, csq_node *body);
csq_node *ast_create_while(csq_node *condition, csq_node *body);
csq_node *ast_create_for(csq_node *var, csq_node *iterable, csq_node *body);
csq_node *ast_create_repeat(csq_node *body, csq_node *condition);
csq_node *ast_create_return(csq_node *value);
csq_node *ast_create_throw(csq_node *value);
csq_node *ast_create_try(csq_node *try_block, node_list *catches);
csq_node *ast_create_catch(csq_node *var, csq_node *type_spec, csq_node *block);
csq_node *ast_create_defer(csq_node *stmt);
csq_node *ast_create_break(void);
csq_node *ast_create_continue(void);
csq_node *ast_create_binary(binary_op op, csq_node *left, csq_node *right);
csq_node *ast_create_unary(unary_op op, csq_node *operand);
csq_node *ast_create_call(csq_node *callee, node_list *args);
csq_node *ast_create_index(csq_node *object, csq_node *index);
csq_node *ast_create_access(csq_node *object, csq_node *member, bool is_arrow);
csq_node *ast_create_identifier(const char *name, size_t len);
csq_node *ast_create_literal_int(long long value);
csq_node *ast_create_literal_float(double value);
csq_node *ast_create_literal_string(const char *value, size_t len);
csq_node *ast_create_literal_bool(bool value);
csq_node *ast_create_literal_nil(void);
csq_node *ast_create_literal_tag(const char *name, size_t len);
csq_node *ast_create_array_literal(node_list *elements);
csq_node *ast_create_map_literal(node_list *entries);
csq_node *ast_create_map_entry(csq_node *key, csq_node *value);
csq_node *ast_create_spawn(csq_node *call_expr);
csq_node *ast_create_import(const char *path, size_t len, csq_node *alias,
                            bool is_wildcard);
csq_node *ast_create_self(void);
csq_node *ast_create_cast(csq_node *expr, csq_node *target_type);
csq_node *ast_create_type_spec(csq_node *base, node_list *args);
csq_node *ast_create_generic_inst(csq_node *base, node_list *type_args);
csq_node *ast_create_range(csq_node *start, csq_node *end, bool inclusive);
csq_node *ast_create_tuple(node_list *elements);
csq_node *ast_create_expr_stmt(csq_node *expr);
csq_node *ast_create_empty(void);

const char *node_type_to_string(node_type type);
const char *binary_op_to_string(binary_op op);
const char *unary_op_to_string(unary_op op);

#endif
