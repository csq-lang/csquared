/**
 * @file cse.c
 * @brief Common subexpression elimination
 * @details Identifies and eliminates redundant expression evaluations.
 */

#include <middle/cse.h>
#include <parser/node.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief Create CSE table
 * @return Allocated CSE table
 */
cse_table* cse_table_create(void)
{
    cse_table* table = malloc(sizeof(cse_table));
    if (!table)
        return NULL;
    
    table->capacity = 64;
    table->entries = malloc(sizeof(cse_entry) * table->capacity);
    if (!table->entries) {
        free(table);
        return NULL;
    }
    
    table->count = 0;
    return table;
}

void cse_table_free(cse_table* table)
{
    if (table) {
        free(table->entries);
        free(table);
    }
}

static int cse_expr_equal(csq_node* a, csq_node* b)
{
    if (!a || !b)
        return a == b;
    
    if (a->type != b->type)
        return 0;
    
    if (a->type == NODE_BINARY_OP) {
        return a->data.binary.op == b->data.binary.op &&
               cse_expr_equal(a->data.binary.left, b->data.binary.left) &&
               cse_expr_equal(a->data.binary.right, b->data.binary.right);
    }
    
    if (a->type == NODE_UNARY_OP) {
        return a->data.unary.op == b->data.unary.op &&
               cse_expr_equal(a->data.unary.operand, b->data.unary.operand);
    }
    
    if (a->type == NODE_CALL) {
        return cse_expr_equal(a->data.call.callee, b->data.call.callee);
    }
    
    if (a->type == NODE_LITERAL_INT) {
        return a->data.literal_int.value == b->data.literal_int.value;
    }
    
    if (a->type == NODE_LITERAL_FLOAT) {
        return a->data.literal_float.value == b->data.literal_float.value;
    }
    
    if (a->type == NODE_IDENTIFIER) {
        return a->data.identifier.len == b->data.identifier.len &&
               !strncmp(a->data.identifier.name, b->data.identifier.name, a->data.identifier.len);
    }
    
    return 0;
}

int cse_table_insert(cse_table* table, csq_node* expr, csq_node* temp)
{
    if (!table || !expr || !temp)
        return 0;
    
    for (int i = 0; i < table->count; i++) {
        if (cse_expr_equal(table->entries[i].expr, expr))
            return 0;
    }
    
    if (table->count >= table->capacity) {
        table->capacity *= 2;
        cse_entry* new_entries = realloc(table->entries, sizeof(cse_entry) * table->capacity);
        if (!new_entries)
            return 0;
        table->entries = new_entries;
    }
    
    table->entries[table->count].expr = expr;
    table->entries[table->count].temp_var = temp;
    table->count++;
    
    return 1;
}

csq_node* cse_table_lookup(cse_table* table, csq_node* expr)
{
    if (!table || !expr)
        return NULL;
    
    for (int i = 0; i < table->count; i++) {
        if (cse_expr_equal(table->entries[i].expr, expr))
            return table->entries[i].temp_var;
    }
    
    return NULL;
}

csq_node* common_subexpr_eliminate(csq_node* node)
{
    if (!node)
        return node;
    
    cse_table* table = cse_table_create();
    if (!table)
        return node;
    
    if (node->type == NODE_BINARY_OP) {
        node->data.binary.left = common_subexpr_eliminate(node->data.binary.left);
        node->data.binary.right = common_subexpr_eliminate(node->data.binary.right);
    }
    
    if (node->type == NODE_UNARY_OP) {
        node->data.unary.operand = common_subexpr_eliminate(node->data.unary.operand);
    }
    
    cse_table_free(table);
    return node;
}
