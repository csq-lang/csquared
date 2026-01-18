#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "raven/error.h"
#include "raven/lexer/lexer.h"
#include "raven/macros.h"
#include "raven/parser/parser.h"
#include "raven/parser/tree.h"

#define currtok (tokens->tokens[index])
#define tok_txt_is(TXT) (strcmp(currtok->txt, TXT) == 0)
#define tok_type_is(TYPE) (currtok->type == TYPE)
#define expect(TXT)                                                            \
  if (!tok_txt_is(TXT)) {                                                      \
    THROW("Expected token '%s', got '%s'.", TXT, currtok->txt);                \
  }
#define skip()                                                                 \
  index++;                                                                     \
  if (index > (size_t)tokens->token_num) {                                     \
    THROW("Out of bounds when parsing. Expected more tokens.");                \
  }
#define expectskip(TXT)                                                        \
  expect(TXT);                                                                 \
  skip();

tree_t *parse(token_list_t *tokens) {
  char rootstr[1];
  rootstr[0] = '\0';
  tree_t *root = new_tree(TREE_ROOT, rootstr, "Root");

  for (size_t i = 0; i <= (size_t)tokens->token_num;) {
    add_child(root, parse_declaration(tokens, i));
  }

  return root;
}

/* Literals */

tree_t *parse_literal(token_list_t *tokens, size_t index) {
  switch (currtok->type) {
  case T_NUMBER:
    skip();
    return new_tree(TREE_NUMBER, currtok->txt, "Number");
    break;
  case T_STRING:
    skip();
    return new_tree(TREE_STRING, currtok->txt, "String");
    break;
  case T_TRUE:
    skip();
    return new_tree(TREE_BOOL_TRUE, currtok->txt, "Bool:True");
    break;
  case T_FALSE:
    skip();
    return new_tree(TREE_BOOL_FALSE, currtok->txt, "Bool:False");
    break;
  case T_TAG:
    skip();
    return new_tree(TREE_TAG, currtok->txt, "Tag");
    break;
  case T_PUNCTUATION: {
    if (tok_txt_is("[")) {
      return parse_literal_array(tokens, index);
    } else if (tok_txt_is("{")) {
      return parse_literal_map(tokens, index);
    } else {
      THROW("Unexpected character '%s' when parsing a literal.", currtok->txt);
    }
    break;
  }
  default:
    THROW("Unexpected token '%s' when parsing a literal.", currtok->txt);
  }
}

tree_t *parse_literal_array(token_list_t *tokens, size_t index) {
  tree_t *array_literal = new_tree(TREE_ARRAY_LITERAL, "", "Array");
  expect("[");

  if (tok_txt_is("]")) {
    return array_literal;
  }

  add_child(array_literal, parse_expr(tokens, index));
  while (tok_txt_is(",")) {
    skip();
    add_child(array_literal, parse_expr(tokens, index));
  }

  expectskip("}");
  return array_literal;
}

tree_t *parse_literal_map_keyvalue(token_list_t *tokens, size_t index) {
  tree_t *keyvalue = new_tree(TREE_MAP_KEYVALUE, "", "Map:KeyValue");

  expectskip("[");

  add_child(keyvalue, parse_expr(tokens, index));

  expectskip("]");
  expectskip(":");

  add_child(keyvalue, parse_expr(tokens, index));

  return keyvalue;
}

tree_t *parse_literal_map(token_list_t *tokens, size_t index) {
  tree_t *map_literal = new_tree(TREE_MAP_LITERAL, "", "Map");
  expect("{");

  if (tok_txt_is("}")) {
    return map_literal;
  }

  add_child(map_literal, parse_literal_map_keyvalue(tokens, index));
  while (tok_txt_is(",")) {
    skip();
    add_child(map_literal, parse_literal_map_keyvalue(tokens, index));
  }

  return map_literal;
}

/* Types */

tree_t *parse_type(token_list_t *tokens, size_t index) {
  if (tok_type_is(T_ID)) {
    return new_tree(TREE_TYPE, currtok->txt, "Type");
  } else if (tok_txt_is("[")) {
    skip();
    tree_t *array_type = new_tree(TREE_ARRAY_TYPE, "", "Type:Array");
    add_child(array_type, parse_type(tokens, index));

    expect("]");
    return array_type;
  } else if (tok_txt_is("{")) {
    skip();
    tree_t *map_type = new_tree(TREE_MAP_TYPE, "", "Type:Map");
    add_child(map_type, parse_type(tokens, index));
    expect(",");
    add_child(map_type, parse_type(tokens, index));

    expect("}");
    return map_type;
  } else if (tok_txt_is("(")) {
    skip();
    tree_t *func_type = new_tree(TREE_FUNC_TYPE, "", "Type:Func");
    if (!tok_txt_is(")")) {
      add_child(func_type, parse_type(tokens, index));
      while (tok_txt_is(",")) {
        skip();
        add_child(func_type, parse_type(tokens, index));
        skip();
      }
    }
    add_child(func_type, parse_type(tokens, index));

    return func_type;
  }

  THROW("Invalid type: expected either an identifier, an array type, a map "
        "type or a "
        "function type: got '%s'.",
        currtok->txt);
}

/* Declarations */

tree_t *parse_declaration(token_list_t *tokens, size_t index) {
  switch (currtok->type) {
  case T_LET:
  case T_CONST:
    return parse_decl_variable(tokens, index);
    break;
  case T_FUNCTION:
  case T_DECORATOR:
    return parse_decl_function(tokens, index);
    break;
  case T_STRUCT:
    return parse_decl_struct(tokens, index);
    break;
  case T_ENUM:
    return parse_decl_enum(tokens, index);
    break;
  default:
    THROW("Unexpected token '%s' when parsing declaration.", currtok->txt);
  }
}

tree_t *parse_decl_variable(token_list_t *tokens, size_t index) {
  tree_t *var_decl = new_tree(TREE_VAR_DECL, "", "VarDecl");
  add_child(var_decl,
            new_tree(TREE_VAR_DECL_MUTB, currtok->txt, "VarDecl:Mutability"));
  skip();

  if (tok_type_is(T_ID)) {
    add_child(var_decl,
              new_tree(TREE_VAR_DECL_NAME, currtok->txt, "VarDecl:Name"));
    skip();
  } else {
    THROW("Variable declaration has no name.");
  }

  if (tok_txt_is(":")) {
    skip();
    add_child(var_decl, parse_type(tokens, index));
  }

  expectskip("=");

  add_child(var_decl, parse_expr(tokens, index));
  return var_decl;
}

tree_t *parse_decl_function_param(token_list_t *tokens, size_t index) {
  if (tok_txt_is("self")) {
    return new_tree(TREE_FUNC_DECL_SELF, "self", "FuncDecl:Param:Self");
  } else if (tok_txt_is("...")) {
    skip();
    tree_t *varparam =
        new_tree(TREE_FUNC_DECL_VAR_PARAM, "", "FuncDecl:Param:Variable");
    if (tok_type_is(T_ID)) {
      add_child(varparam, new_tree(TREE_FUNC_DECL_PARAM_NAME, currtok->txt,
                                   "FuncDecl:Param:Name"));
      skip();
    } else {
      THROW("Function variable parameter has no name.");
    }

    expectskip(":");

    add_child(varparam, parse_type(tokens, index));

    return varparam;
  } else if (tok_type_is(T_ID)) {
    tree_t *param = new_tree(TREE_FUNC_DECL_PARAM, "", "FuncDecl:Param");
    add_child(param, new_tree(TREE_FUNC_DECL_PARAM_NAME, currtok->txt,
                              "FuncDecl:Param:Name"));

    expectskip(":");

    add_child(param, parse_type(tokens, index));
    skip();

    if (tok_txt_is("=")) {
      skip();
      tree_t *default_val =
          new_tree(TREE_FUNC_DECL_PARAM_DEFAULT, "", "FuncDecl:Param:Default");
      add_child(default_val, parse_expr(tokens, index));
      add_child(param, default_val);
    }

    return param;
  }

  THROW("Invalid function parameter: expected either 'self', a variable "
        "parameter or a normal parameter; got '%s'",
        currtok->txt);
}

tree_t *parse_decl_function(token_list_t *tokens, size_t index) {
  tree_t *func_decl = new_tree(TREE_FUNC_DECL, "", "FuncDecl");

  while (tok_type_is(T_DECORATOR)) {
    add_child(func_decl, new_tree(TREE_FUNC_DECL_DECO, currtok->txt,
                                  "FuncDecl:Decorator"));
    skip();
  }

  expectskip("function");

  if (tok_type_is(T_ID)) {
    add_child(func_decl,
              new_tree(TREE_FUNC_DECL_NAME, currtok->txt, "FuncDecl:Name"));
    skip();
  } else {
    THROW("Function declaration has no name.");
  }

  expectskip("(");

  if (tok_txt_is(")")) {
    goto func_block;
  }

  add_child(func_decl, parse_decl_function_param(tokens, index));
  while (tok_txt_is(",")) {
    skip();
    add_child(func_decl, parse_decl_function_param(tokens, index));
    skip();
  }

func_block:

  add_child(func_decl, parse_type(tokens, index));
  expectskip("{");
  while (!tok_txt_is("}")) {
    add_child(func_decl, parse_statement(tokens, index));
  }
  expectskip("}");

  return func_decl;
}

// tree_t *parse_decl_struct_field(token_list_t *tokens, size_t index);
// tree_t *parse_decl_struct(token_list_t *tokens, size_t index);

// tree_t* parse_decl_enum(token_list_t *tokens, size_t index);

/* TODO */

#define TODO_PARSE_FUNC(name)                                                  \
  tree_t *name(token_list_t *tokens, size_t index) {                           \
    UNUSED(tokens);                                                            \
    UNUSED(index);                                                             \
    TODO("function '" #name "'");                                              \
  }

TODO_PARSE_FUNC(parse_expr)
TODO_PARSE_FUNC(parse_statement)
TODO_PARSE_FUNC(parse_decl_struct)
TODO_PARSE_FUNC(parse_decl_struct_field)
TODO_PARSE_FUNC(parse_decl_enum)
