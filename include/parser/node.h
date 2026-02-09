/**
 * @file node.h
 * @brief Abstract Syntax Tree (AST) node definitions for the C² parser.
 * @details Defines all node types, operators, and data structures used to
 * represent the parsed program structure. Provides interfaces for AST node
 * creation and manipulation.
 */

#ifndef PARSER_NODE_H
#define PARSER_NODE_H

#include <csquare/token.h>
#include <stdbool.h>
#include <stddef.h>

typedef struct csq_node csq_node;

/**
 * @brief Node type enumeration.
 * @details Categorizes all possible AST node types representing different
 * language constructs from program structure to expressions and statements.
 */
typedef enum {
  NODE_PROGRAM,        /**< Program root node */
  NODE_BLOCK,          /**< Block of statements */
  NODE_VAR_DECL,       /**< Variable declaration */
  NODE_CONST_DECL,     /**< Constant declaration */
  NODE_FUNCTION_DECL,  /**< Function declaration */
  NODE_PARAM,          /**< Function parameter */
  NODE_STRUCT_DECL,    /**< Struct definition */
  NODE_ENUM_DECL,      /**< Enumeration definition */
  NODE_ENUM_VARIANT,   /**< Enum variant/value */
  NODE_FIELD,          /**< Struct field */
  NODE_IF,             /**< If statement */
  NODE_SWITCH,         /**< Switch statement */
  NODE_CASE,           /**< Case clause in switch */
  NODE_WHILE,          /**< While loop */
  NODE_FOR,            /**< For loop */
  NODE_REPEAT,         /**< Repeat loop */
  NODE_RETURN,         /**< Return statement */
  NODE_THROW,          /**< Throw exception statement */
  NODE_TRY,            /**< Try block */
  NODE_CATCH,          /**< Catch clause */
  NODE_DEFER,          /**< Defer statement */
  NODE_BREAK,          /**< Break statement */
  NODE_CONTINUE,       /**< Continue statement */
  NODE_ASSIGN,         /**< Assignment expression */
  NODE_BINARY_OP,      /**< Binary operation */
  NODE_UNARY_OP,       /**< Unary operation */
  NODE_CALL,           /**< Function call */
  NODE_INDEX,          /**< Array indexing */
  NODE_ACCESS,         /**< Member access */
  NODE_IDENTIFIER,     /**< Variable identifier */
  NODE_LITERAL_INT,    /**< Integer literal */
  NODE_LITERAL_FLOAT,  /**< Floating-point literal */
  NODE_LITERAL_STRING, /**< String literal */
  NODE_LITERAL_BOOL,   /**< Boolean literal */
  NODE_LITERAL_NIL,    /**< Nil literal */
  NODE_LITERAL_TAG,    /**< Tag literal */
  NODE_ARRAY_LITERAL,  /**< Array literal */
  NODE_MAP_LITERAL,    /**< Map/dictionary literal */
  NODE_MAP_ENTRY,      /**< Single map key-value entry */
  NODE_SPAWN,          /**< Spawn/async task */
  NODE_IMPORT,         /**< Import statement */
  NODE_SELF,           /**< Self reference */
  NODE_CAST,           /**< Type cast expression */
  NODE_TYPE_SPEC,      /**< Type specification */
  NODE_GENERIC_INST,   /**< Generic type instantiation */
  NODE_RANGE,          /**< Range expression */
  NODE_TUPLE,          /**< Tuple literal */
  NODE_EXPR_STMT,      /**< Expression statement */
  NODE_EMPTY           /**< Empty/placeholder node */
} node_type;

/**
 * @brief Binary operator enumeration.
 * @details Represents all binary operators supported by the language,
 * including arithmetic, comparison, logical, and bitwise operations.
 */
typedef enum {
  BINOP_ADD,        /**< Addition operator */
  BINOP_SUB,        /**< Subtraction operator */
  BINOP_MUL,        /**< Multiplication operator */
  BINOP_DIV,        /**< Division operator */
  BINOP_MOD,        /**< Modulo operator */
  BINOP_POW,        /**< Power operator */
  BINOP_EQ,         /**< Equality operator */
  BINOP_NE,         /**< Not-equal operator */
  BINOP_LT,         /**< Less-than operator */
  BINOP_GT,         /**< Greater-than operator */
  BINOP_LE,         /**< Less-than-or-equal operator */
  BINOP_GE,         /**< Greater-than-or-equal operator */
  BINOP_AND,        /**< Logical AND operator */
  BINOP_OR,         /**< Logical OR operator */
  BINOP_BIT_AND,    /**< Bitwise AND operator */
  BINOP_BIT_OR,     /**< Bitwise OR operator */
  BINOP_BIT_XOR,    /**< Bitwise XOR operator */
  BINOP_SHL,        /**< Bitwise left shift operator */
  BINOP_SHR,        /**< Bitwise right shift operator */
  BINOP_ASSIGN,     /**< Assignment operator */
  BINOP_ADD_ASSIGN, /**< Addition assignment operator */
  BINOP_SUB_ASSIGN, /**< Subtraction assignment operator */
  BINOP_MUL_ASSIGN, /**< Multiplication assignment operator */
  BINOP_DIV_ASSIGN, /**< Division assignment operator */
  BINOP_MOD_ASSIGN  /**< Modulo assignment operator */
} binary_op;

/**
 * @brief Unary operator enumeration.
 * @details Represents all unary operators supported by the language,
 * including negation, logical NOT, and address-of operations.
 */
typedef enum {
  UNOP_NEG,      /**< Negation operator */
  UNOP_NOT,      /**< Logical NOT operator */
  UNOP_BIT_NOT,  /**< Bitwise NOT operator */
  UNOP_PRE_INC,  /**< Pre-increment operator */
  UNOP_PRE_DEC,  /**< Pre-decrement operator */
  UNOP_POST_INC, /**< Post-increment operator */
  UNOP_POST_DEC, /**< Post-decrement operator */
  UNOP_DEREF,    /**< Dereference operator */
  UNOP_ADDR      /**< Address-of operator */
} unary_op;

/**
 * @brief Dynamic array of AST nodes.
 * @details Growable array used to store collections of child nodes
 * (e.g., statements in a block, parameters in a function).
 */
typedef struct {
  csq_node **items; /**< Array of node pointers */
  size_t count;     /**< Number of items currently stored */
  size_t capacity;  /**< Allocated capacity for items */
} node_list;

struct csq_node {
  node_type type;
  size_t line;
  size_t column;
  struct csq_type *type_info;
  struct csq_symbol *symbol;

  union {
    struct {
      node_list statements;
    } program; /**< Program root node data */

    struct {
      node_list statements;
    } block; /**< Block node data */

    struct {
      csq_node *name;
      csq_node *type_spec;
      csq_node *init;
      bool is_mutable;
    } var_decl; /**< Variable declaration node data */

    struct {
      csq_node *name;
      csq_node *type_spec;
      csq_node *init;
    } const_decl; /**< Constant declaration node data */

    struct {
      csq_node *name;
      node_list params;
      csq_node *return_type;
      csq_node *body;
      node_list generics;
      bool is_async;
    } function_decl; /**< Function declaration node data */

    struct {
      csq_node *name;
      csq_node *type_spec;
      csq_node *default_val;
    } param; /**< Function parameter node data */

    struct {
      csq_node *name;
      node_list fields;
      node_list methods;
      node_list generics;
    } struct_decl; /**< Struct declaration node data */

    struct {
      csq_node *name;
      node_list variants;
    } enum_decl; /**< Enumeration declaration node data */

    struct {
      csq_node *name;
      node_list fields;
      int value;
    } enum_variant; /**< Enum variant node data */

    struct {
      csq_node *name;
      csq_node *type_spec;
    } field; /**< Struct field node data */

    struct {
      csq_node *condition;
      csq_node *then_branch;
      csq_node *else_branch;
    } if_stmt; /**< If statement node data */

    struct {
      csq_node *expr;
      node_list cases;
      csq_node *default_case;
    } switch_stmt; /**< Switch statement node data */

    struct {
      csq_node *value;
      csq_node *body;
    } case_stmt; /**< Case statement node data */

    struct {
      csq_node *condition;
      csq_node *body;
    } while_stmt; /**< While loop node data */

    struct {
      csq_node *var;
      csq_node *iterable;
      csq_node *body;
    } for_stmt; /**< For loop node data */

    struct {
      csq_node *body;
      csq_node *condition;
    } repeat_stmt; /**< Repeat loop node data */

    struct {
      csq_node *value;
    } return_stmt; /**< Return statement node data */

    struct {
      csq_node *value;
    } throw_stmt; /**< Throw statement node data */

    struct {
      csq_node *try_block;
      node_list catch_blocks;
    } try_stmt; /**< Try statement node data */

    struct {
      csq_node *var;
      csq_node *type_spec;
      csq_node *block;
    } catch_block; /**< Catch clause node data */

    struct {
      csq_node *stmt;
    } defer_stmt; /**< Defer statement node data */

    struct {
      binary_op op;
      csq_node *left;
      csq_node *right;
    } binary; /**< Binary operation node data */

    struct {
      unary_op op;
      csq_node *operand;
    } unary; /**< Unary operation node data */

    struct {
      csq_node *callee;
      node_list args;
    } call; /**< Function call node data */

    struct {
      csq_node *object;
      csq_node *index;
    } index; /**< Array indexing node data */

    struct {
      csq_node *object;
      csq_node *member;
      bool is_arrow;
    } access; /**< Member access node data */

    struct {
      const char *name;
      size_t len;
    } identifier; /**< Identifier node data */

    struct {
      long long value;
    } literal_int; /**< Integer literal node data */

    struct {
      double value;
    } literal_float; /**< Float literal node data */

    struct {
      const char *value;
      size_t len;
    } literal_string; /**< String literal node data */

    struct {
      bool value;
    } literal_bool; /**< Boolean literal node data */

    struct {
      const char *name;
      size_t len;
    } literal_tag; /**< Tag literal node data */

    struct {
      node_list elements;
    } array_literal; /**< Array literal node data */

    struct {
      node_list entries;
    } map_literal; /**< Map literal node data */

    struct {
      csq_node *key;
      csq_node *value;
    } map_entry; /**< Map entry node data */

    struct {
      csq_node *call_expr;
    } spawn; /**< Spawn/async node data */

    struct {
      const char *path;
      size_t len;
      csq_node *alias;
      bool is_wildcard;
    } import; /**< Import statement node data */

    struct {
      csq_node *expr;
      csq_node *target_type;
    } cast; /**< Type cast node data */

    struct {
      csq_node *base;
      node_list args;
      bool is_optional;
    } type_spec; /**< Type specification node data */

    struct {
      csq_node *base;
      node_list type_args;
    } generic_inst; /**< Generic instantiation node data */

    struct {
      csq_node *start;
      csq_node *end;
      bool inclusive;
    } range;

    struct {
      node_list elements;
    } tuple; /**< Tuple node data */

    struct {
      csq_node *expr;
    } expr_stmt; /**< Expression statement node data */
  } data;        /**< Union containing type-specific node data */
};

/**
 * @brief Creates a new AST node of the specified type.
 * @details Allocates memory for a new node and initializes its basic fields.
 * The node's union data field is not initialized.
 * @param type The type of node to create.
 * @param line Source line number.
 * @param column Source column number.
 * @return Pointer to newly allocated node, or NULL on memory error.
 */
csq_node *node_create(node_type type, size_t line, size_t column);

/**
 * @brief Frees an AST node and its children recursively.
 * @details Deallocates the node and all child nodes referenced by it.
 * @param node The node to free. Safe to call with NULL.
 */
void node_free(csq_node *node);

/**
 * @brief Initializes a node list.
 * @details Prepares an empty node list for adding nodes.
 * @param list Pointer to the list to initialize.
 */
void node_list_init(node_list *list);

/**
 * @brief Frees all nodes in a list and the list itself.
 * @details Deallocates all nodes and the list structure.
 * @param list Pointer to the list to free.
 */
void node_list_free(node_list *list);

/**
 * @brief Adds a node to a list.
 * @details Appends a node to the end of the list, growing capacity as needed.
 * @param list Pointer to the list.
 * @param node The node to add.
 */
void node_list_add(node_list *list, csq_node *node);

#endif
