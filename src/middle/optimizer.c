/**
 * @file optimizer.c
 * @brief Optimization framework and management
 * @details Orchestrates multiple optimization passes including constant folding,
 * dead code elimination, common subexpression elimination, and strength reduction.
 */

#include <middle/optimizer.h>
#include <middle/const_fold.h>
#include <middle/dead_code.h>
#include <middle/cse.h>
#include <middle/strength_reduce.h>
#include <parser/ast.h>
#include <stdlib.h>

/**
 * @brief Create optimizer state
 * @param ast AST to optimize
 * @return Allocated optimizer state
 */
optimizer_state* optimizer_create(ast_context* ast)
{
    optimizer_state* opt = malloc(sizeof(optimizer_state));
    if (!opt)
        return NULL;
    
    opt->ast = ast;
    opt->changed = 0;
    
    opt->config.enable_const_folding = 1;
    opt->config.enable_dce = 1;
    opt->config.enable_cse = 1;
    opt->config.enable_strength_reduction = 1;
    opt->config.enable_inlining = 0;
    opt->config.enable_loop_unroll = 0;
    opt->config.enable_alias_analysis = 0;
    opt->config.opt_level = 2;
    
    return opt;
}

/**
 * @brief Free optimizer state
 * @param opt Optimizer to deallocate
 */
void optimizer_free(optimizer_state* opt)
{
    if (opt)
        free(opt);
}

/**
 * @brief Configure optimization level
 * @param opt Optimizer state
 * @param opt_level Optimization level (0-2+)
 */
void optimizer_configure(optimizer_state* opt, int opt_level)
{
    if (!opt)
        return;
    
    opt->config.opt_level = opt_level;
    
    if (opt_level == 0) {
        opt->config.enable_const_folding = 0;
        opt->config.enable_dce = 0;
        opt->config.enable_cse = 0;
        opt->config.enable_strength_reduction = 0;
        opt->config.enable_inlining = 0;
        opt->config.enable_loop_unroll = 0;
    } else if (opt_level == 1) {
        opt->config.enable_const_folding = 1;
        opt->config.enable_dce = 1;
        opt->config.enable_cse = 0;
        opt->config.enable_strength_reduction = 1;
        opt->config.enable_inlining = 0;
        opt->config.enable_loop_unroll = 0;
    } else if (opt_level >= 2) {
        opt->config.enable_const_folding = 1;
        opt->config.enable_dce = 1;
        opt->config.enable_cse = 1;
        opt->config.enable_strength_reduction = 1;
        opt->config.enable_inlining = 1;
        opt->config.enable_loop_unroll = 1;
    }
}

static csq_node* optimizer_visit(csq_node* node, optimizer_state* opt)
{
    if (!node)
        return node;
    
    csq_node* result = node;
    
    if (opt->config.enable_const_folding) {
        result = const_fold_apply(result);
        if (result != node)
            opt->changed = 1;
    }
    
    if (opt->config.enable_strength_reduction) {
        result = strength_reduce_apply(result);
        if (result != node)
            opt->changed = 1;
    }
    
    return result;
}

static void optimizer_walk_ast(csq_node* node, optimizer_state* opt)
{
    if (!node)
        return;
    
    if (node->type == NODE_BINARY_OP) {
        optimizer_walk_ast(node->data.binary.left, opt);
        optimizer_walk_ast(node->data.binary.right, opt);
    } else if (node->type == NODE_UNARY_OP) {
        optimizer_walk_ast(node->data.unary.operand, opt);
    }
    
    optimizer_visit(node, opt);
}

int optimizer_run(optimizer_state* opt)
{
    if (!opt || !opt->ast)
        return 0;
    
    int pass = 0;
    int max_passes = 5;
    
    while (pass < max_passes) {
        opt->changed = 0;
        
        if (opt->ast->root)
            optimizer_walk_ast(opt->ast->root, opt);
        
        if (opt->config.enable_dce && opt->ast->root) {
            csq_node* result = dead_code_eliminate(opt->ast->root);
            if (result != opt->ast->root)
                opt->changed = 1;
            opt->ast->root = result;
        }
        
        if (!opt->changed)
            break;
        
        pass++;
    }
    
    return pass;
}
