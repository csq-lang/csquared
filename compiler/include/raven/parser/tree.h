#ifndef INCLUDE_INCLUDE_TREE_H_
#define INCLUDE_INCLUDE_TREE_H_

#include <stddef.h>

typedef enum {
  TREE_ROOT,

  /* Primitive literals */
  TREE_NUMBER,
  TREE_STRING,
  TREE_BOOL_TRUE,
  TREE_BOOL_FALSE,
  TREE_TAG,

  /* Array literal */
  TREE_ARRAY_LITERAL,

  /* Map literal */
  TREE_MAP_LITERAL,
  TREE_MAP_KEYVALUE,

  /* Primitive types */
  TREE_TYPE,

  /* Structural types */
  TREE_ARRAY_TYPE,
  TREE_MAP_TYPE,

  /* Function type */
  TREE_FUNC_TYPE,
  TREE_FUNC_TYPE_ARGS,

  /* Variable declaration */
  TREE_VAR_DECL,
  TREE_VAR_DECL_MUTB,
  TREE_VAR_DECL_NAME,

  /* Function declaration */
  TREE_FUNC_DECL,
  TREE_FUNC_DECL_DECO,
  TREE_FUNC_DECL_NAME,

  /* Function parameter */
  TREE_FUNC_DECL_PARAM,
  TREE_FUNC_DECL_PARAM_NAME,
  TREE_FUNC_DECL_PARAM_TYPE,
  TREE_FUNC_DECL_PARAM_DEFAULT,
  TREE_FUNC_DECL_VAR_PARAM,
  TREE_FUNC_DECL_SELF,

} nodetype_t;

typedef struct tree_t tree_t;

struct tree_t {
  nodetype_t type;
  char *data;
  char *label; // for debug purpose

  tree_t **children;
  size_t child_count;
};

tree_t *new_tree(nodetype_t type, char *data, const char *label);
/* Frees the tree + all children, which means that
 * the WHOLE tree top-to-bottom is freed. Should only be used on
 * the tree's root to avoid double frees!
 */
void free_tree(tree_t *tree);
void add_child(tree_t *tree, tree_t *child);

int tree_contains_child(tree_t *tree, nodetype_t expected);
int tree_get_child_index(tree_t *tree, nodetype_t expected);

#endif // INCLUDE_INCLUDE_TREE_H_
