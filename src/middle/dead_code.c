/**
 * @file dead_code.c
 * @brief Dead code elimination
 * @details Identifies and marks unused variables and unreachable code.
 */

#include <middle/dead_code.h>
#include <parser/node.h>
#include <stdlib.h>

/**
 * @brief Create live variable analysis state
 * @return Allocated live analysis structure
 */
live_analysis* live_analysis_create(void)
{
    live_analysis* la = malloc(sizeof(live_analysis));
    if (!la)
        return NULL;
    
    la->capacity = 32;
    la->live_vars = malloc(sizeof(csq_node*) * la->capacity);
    if (!la->live_vars) {
        free(la);
        return NULL;
    }
    
    la->live_count = 0;
    return la;
}

void live_analysis_free(live_analysis* la)
{
    if (la) {
        free(la->live_vars);
        free(la);
    }
}

int live_analysis_run(csq_node* node, live_analysis* la)
{
    if (!node || !la)
        return 0;
    
    return 0;
}

static void dead_code_mark_used(csq_node* node, live_analysis* la)
{
    if (!node || !la)
        return;
    
    if (node->type == NODE_IDENTIFIER) {
        for (int i = 0; i < la->live_count; i++) {
            if (la->live_vars[i] == node)
                return;
        }
        
        if (la->live_count >= la->capacity) {
            la->capacity *= 2;
            csq_node** new_vars = realloc(la->live_vars, sizeof(csq_node*) * la->capacity);
            if (!new_vars)
                return;
            la->live_vars = new_vars;
        }
        
        la->live_vars[la->live_count++] = node;
    }
    
    if (node->type == NODE_BINARY_OP) {
        dead_code_mark_used(node->data.binary.left, la);
        dead_code_mark_used(node->data.binary.right, la);
    }
    
    if (node->type == NODE_UNARY_OP) {
        dead_code_mark_used(node->data.unary.operand, la);
    }
}

static int dead_code_is_used(csq_node* node, live_analysis* la)
{
    if (!node || !la)
        return 1;
    
    if (node->type != NODE_IDENTIFIER)
        return 1;
    
    for (int i = 0; i < la->live_count; i++) {
        if (la->live_vars[i] == node)
            return 1;
    }
    
    return 0;
}

static csq_node* dead_code_eliminate_stmt(csq_node* node, live_analysis* la)
{
    if (!node)
        return node;
    
    if (node->type == NODE_VAR_DECL) {
        if (!dead_code_is_used(node, la))
            return NULL;
    }
    
    return node;
}

csq_node* dead_code_eliminate(csq_node* node)
{
    if (!node)
        return node;
    
    live_analysis* la = live_analysis_create();
    if (!la)
        return node;
    
    dead_code_mark_used(node, la);
    csq_node* result = dead_code_eliminate_stmt(node, la);
    
    live_analysis_free(la);
    return result;
}
