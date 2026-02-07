/**
 * @file gen_binary.c
 * @brief Binary operation code generation
 * @details Generates x86 assembly code for binary operations including
 * arithmetic, bitwise, logical, and assignment operations.
 */

#include <codegen/x86/gen_binary.h>
#include <codegen/x86/gen_expr.h>
#include <codegen/x86/gen_const.h>
#include <codegen/x86/gen_var.h>
#include <codegen/x86/gen_utils.h>
#include <codegen/x86/inst.h>
#include <codegen/x86/label.h>
#include <parser/node.h>
#include <parser/types.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

static void gen_handle_assignment(gen_ctx* ctx, csq_node* node) {
    if (node->data.binary.left->type == NODE_INDEX) {
        gen_expression(ctx, node->data.binary.left->data.index.object);
        inst_push_reg(ctx->emit, REG_EAX);
        
        gen_expression(ctx, node->data.binary.left->data.index.index);
        inst_push_reg(ctx->emit, REG_EAX);
        
        if (node->data.binary.op == BINOP_ASSIGN) {
            gen_expression(ctx, node->data.binary.right);
        } else {
            emit_indent(ctx->emit);
            emit_line(ctx->emit, "movl (%%eax, %%ebx, 4), %%ecx");
            gen_expression(ctx, node->data.binary.right);
            inst_mov_reg_reg(ctx->emit, REG_EDX, REG_EAX);
            inst_mov_reg_reg(ctx->emit, REG_EAX, REG_ECX);
            
            switch (node->data.binary.op) {
                case BINOP_ADD_ASSIGN:
                    inst_add_reg_reg(ctx->emit, REG_EAX, REG_EDX);
                    break;
                case BINOP_SUB_ASSIGN:
                    inst_sub_reg_reg(ctx->emit, REG_EAX, REG_EDX);
                    break;
                case BINOP_MUL_ASSIGN:
                    inst_imul_reg_reg(ctx->emit, REG_EAX, REG_EDX);
                    break;
                case BINOP_DIV_ASSIGN:
                    inst_mov_reg_reg(ctx->emit, REG_EAX, REG_ECX);
                    inst_cdq(ctx->emit);
                    inst_idiv_reg(ctx->emit, REG_EDX);
                    break;
                case BINOP_MOD_ASSIGN:
                    inst_mov_reg_reg(ctx->emit, REG_EAX, REG_ECX);
                    inst_cdq(ctx->emit);
                    inst_idiv_reg(ctx->emit, REG_EDX);
                    inst_mov_reg_reg(ctx->emit, REG_EAX, REG_EDX);
                    break;
                default:
                    break;
            }
        }
        
        inst_pop_reg(ctx->emit, REG_EBX);
        inst_pop_reg(ctx->emit, REG_EAX);
        
        emit_line(ctx->emit, "movl %%eax, (%%eax, %%ebx, 4)");
    } else if (node->data.binary.left->type == NODE_IDENTIFIER) {
        const char* name = node->data.binary.left->data.identifier.name;
        var_info* var = var_table_find(ctx->vars, name);
        
        if (node->data.binary.op == BINOP_ASSIGN) {
            gen_expression(ctx, node->data.binary.right);
        } else if (var) {
            emit_line(ctx->emit, "movl %d(%%ebp), %%ecx", var->offset);
            gen_expression(ctx, node->data.binary.right);
            inst_mov_reg_reg(ctx->emit, REG_EDX, REG_EAX);
            inst_mov_reg_reg(ctx->emit, REG_EAX, REG_ECX);
            
            switch (node->data.binary.op) {
                case BINOP_ADD_ASSIGN:
                    inst_add_reg_reg(ctx->emit, REG_EAX, REG_EDX);
                    break;
                case BINOP_SUB_ASSIGN:
                    inst_sub_reg_reg(ctx->emit, REG_EAX, REG_EDX);
                    break;
                case BINOP_MUL_ASSIGN:
                    inst_imul_reg_reg(ctx->emit, REG_EAX, REG_EDX);
                    break;
                case BINOP_DIV_ASSIGN:
                    inst_mov_reg_reg(ctx->emit, REG_EAX, REG_ECX);
                    inst_cdq(ctx->emit);
                    inst_idiv_reg(ctx->emit, REG_EDX);
                    break;
                case BINOP_MOD_ASSIGN:
                    inst_mov_reg_reg(ctx->emit, REG_EAX, REG_ECX);
                    inst_cdq(ctx->emit);
                    inst_idiv_reg(ctx->emit, REG_EDX);
                    inst_mov_reg_reg(ctx->emit, REG_EAX, REG_EDX);
                    break;
                default:
                    break;
            }
            inst_mov_mem_reg(ctx->emit, REG_EBP, var->offset, REG_EAX);
            return;
        } else {
            return;
        }
    }
}

static bool gen_is_string_concat(gen_ctx* ctx, csq_node* node) {
    (void)ctx;
    if (node->data.binary.left->type == NODE_LITERAL_STRING ||
        node->data.binary.right->type == NODE_LITERAL_STRING) {
        return true;
    }
    
    csq_type* left_type = node->data.binary.left->type_info;
    csq_type* right_type = node->data.binary.right->type_info;
    if ((left_type && left_type->kind == TYPE_STRING) ||
        (right_type && right_type->kind == TYPE_STRING)) {
        return true;
    }
    
    return false;
}

static void gen_string_concatenation(gen_ctx* ctx, csq_node* node) {
    gen_expression(ctx, node->data.binary.left);
    inst_push_reg(ctx->emit, REG_EAX);
    
    emit_raw(ctx->emit, "    movl %eax, %edi\n");
    emit_raw(ctx->emit, "    xorl %ecx, %ecx\n");
    emit_raw(ctx->emit, "strlen_left_loop:\n");
    emit_raw(ctx->emit, "    movb (%edi), %al\n");
    emit_raw(ctx->emit, "    testb %al, %al\n");
    emit_raw(ctx->emit, "    je strlen_left_done\n");
    emit_raw(ctx->emit, "    incl %ecx\n");
    emit_raw(ctx->emit, "    incl %edi\n");
    emit_raw(ctx->emit, "    jmp strlen_left_loop\n");
    emit_raw(ctx->emit, "strlen_left_done:\n");
    emit_raw(ctx->emit, "    movl %ecx, %ebx\n");
    
    inst_pop_reg(ctx->emit, REG_EAX);
    inst_push_reg(ctx->emit, REG_EAX);
    
    gen_expression(ctx, node->data.binary.right);
    inst_push_reg(ctx->emit, REG_EAX);
    
    emit_raw(ctx->emit, "    movl %eax, %edi\n");
    emit_raw(ctx->emit, "    xorl %ecx, %ecx\n");
    emit_raw(ctx->emit, "strlen_right_loop:\n");
    emit_raw(ctx->emit, "    movb (%edi), %al\n");
    emit_raw(ctx->emit, "    testb %al, %al\n");
    emit_raw(ctx->emit, "    je strlen_right_done\n");
    emit_raw(ctx->emit, "    incl %ecx\n");
    emit_raw(ctx->emit, "    incl %edi\n");
    emit_raw(ctx->emit, "    jmp strlen_right_loop\n");
    emit_raw(ctx->emit, "strlen_right_done:\n");
    
    inst_add_reg_reg(ctx->emit, REG_ECX, REG_EBX);
    inst_inc_reg(ctx->emit, REG_ECX);
    
    emit_raw(ctx->emit, "    movl %esp, %edx\n");
    emit_raw(ctx->emit, "    movl $0, %eax\n");
    emit_raw(ctx->emit, "    movl $45, %ebx\n");
    emit_raw(ctx->emit, "    int $0x80\n");
    emit_raw(ctx->emit, "    addl %ecx, %eax\n");
    emit_raw(ctx->emit, "    movl %eax, %edi\n");
    
    inst_pop_reg(ctx->emit, REG_ECX);
    inst_push_reg(ctx->emit, REG_ECX);
    
    inst_pop_reg(ctx->emit, REG_EAX);
    inst_push_reg(ctx->emit, REG_EAX);
    
    emit_raw(ctx->emit, "    movl %eax, %esi\n");
    emit_raw(ctx->emit, "    movl %edi, %edx\n");
    emit_raw(ctx->emit, "strcpy_loop:\n");
    emit_raw(ctx->emit, "    movb (%esi), %al\n");
    emit_raw(ctx->emit, "    movb %al, (%edx)\n");
    emit_raw(ctx->emit, "    testb %al, %al\n");
    emit_raw(ctx->emit, "    je strcpy_done\n");
    emit_raw(ctx->emit, "    incl %esi\n");
    emit_raw(ctx->emit, "    incl %edx\n");
    emit_raw(ctx->emit, "    jmp strcpy_loop\n");
    emit_raw(ctx->emit, "strcpy_done:\n");
    
    emit_raw(ctx->emit, "    movl %edi, %edx\n");
    emit_raw(ctx->emit, "strcat_find_end:\n");
    emit_raw(ctx->emit, "    movb (%edx), %al\n");
    emit_raw(ctx->emit, "    testb %al, %al\n");
    emit_raw(ctx->emit, "    jne strcat_find_end_next\n");
    emit_raw(ctx->emit, "    jmp strcat_copy_start\n");
    emit_raw(ctx->emit, "strcat_find_end_next:\n");
    emit_raw(ctx->emit, "    incl %edx\n");
    emit_raw(ctx->emit, "    jmp strcat_find_end\n");
    emit_raw(ctx->emit, "strcat_copy_start:\n");
    
    inst_pop_reg(ctx->emit, REG_ESI);
    
    emit_raw(ctx->emit, "strcat_copy_loop:\n");
    emit_raw(ctx->emit, "    movb (%esi), %al\n");
    emit_raw(ctx->emit, "    movb %al, (%edx)\n");
    emit_raw(ctx->emit, "    testb %al, %al\n");
    emit_raw(ctx->emit, "    je strcat_done\n");
    emit_raw(ctx->emit, "    incl %esi\n");
    emit_raw(ctx->emit, "    incl %edx\n");
    emit_raw(ctx->emit, "    jmp strcat_copy_loop\n");
    emit_raw(ctx->emit, "strcat_done:\n");
    
    inst_pop_reg(ctx->emit, REG_EAX);
    inst_mov_reg_reg(ctx->emit, REG_EAX, REG_EDI);
}

static void gen_short_circuit_and(gen_ctx* ctx, csq_node* node) {
    char* false_label = label_gen_anon(ctx->labels);
    char* end_label = label_gen_anon(ctx->labels);
    
    if (!false_label || !end_label) return;
    
    gen_expression(ctx, node->data.binary.left);
    inst_test_reg_reg(ctx->emit, REG_EAX, REG_EAX);
    inst_je_label(ctx->emit, false_label);
    
    gen_expression(ctx, node->data.binary.right);
    inst_test_reg_reg(ctx->emit, REG_EAX, REG_EAX);
    inst_je_label(ctx->emit, false_label);
    
    inst_mov_reg_imm(ctx->emit, REG_EAX, 1);
    inst_jmp_label(ctx->emit, end_label);
    
    emit_label(ctx->emit, false_label);
    inst_mov_reg_imm(ctx->emit, REG_EAX, 0);
    
    emit_label(ctx->emit, end_label);
}

static void gen_short_circuit_or(gen_ctx* ctx, csq_node* node) {
    char* true_label = label_gen_anon(ctx->labels);
    char* end_label = label_gen_anon(ctx->labels);
    
    if (!true_label || !end_label) return;
    
    gen_expression(ctx, node->data.binary.left);
    inst_test_reg_reg(ctx->emit, REG_EAX, REG_EAX);
    inst_jne_label(ctx->emit, true_label);
    
    gen_expression(ctx, node->data.binary.right);
    inst_test_reg_reg(ctx->emit, REG_EAX, REG_EAX);
    inst_jne_label(ctx->emit, true_label);
    
    inst_mov_reg_imm(ctx->emit, REG_EAX, 0);
    inst_jmp_label(ctx->emit, end_label);
    
    emit_label(ctx->emit, true_label);
    inst_mov_reg_imm(ctx->emit, REG_EAX, 1);
    
    emit_label(ctx->emit, end_label);
}

static void gen_optimized_add(gen_ctx* ctx, csq_node* node) {
    gen_expression(ctx, node->data.binary.left);
    
    if (node->data.binary.right->type == NODE_IDENTIFIER) {
        const char* name = node->data.binary.right->data.identifier.name;
        var_info* var = var_table_find(ctx->vars, name);
        if (var) {
            emit_line(ctx->emit, "addl %d(%%ebp), %%eax", var->offset);
            return;
        }
    } else if (node->data.binary.right->type == NODE_LITERAL_INT) {
        int val = (int)node->data.binary.right->data.literal_int.value;
        inst_add_reg_imm(ctx->emit, REG_EAX, val);
        return;
    }
}

void gen_binary_op(gen_ctx* ctx, csq_node* node) {
    if (!node || node->type != NODE_BINARY_OP) return;
    
    if (node->data.binary.op == BINOP_ASSIGN || 
        node->data.binary.op == BINOP_ADD_ASSIGN || 
        node->data.binary.op == BINOP_SUB_ASSIGN || 
        node->data.binary.op == BINOP_MUL_ASSIGN || 
        node->data.binary.op == BINOP_DIV_ASSIGN || 
        node->data.binary.op == BINOP_MOD_ASSIGN) {
        gen_handle_assignment(ctx, node);
        return;
    }
    
    if (node->data.binary.op == BINOP_ADD && gen_is_string_concat(ctx, node)) {
        gen_string_concatenation(ctx, node);
        return;
    }
    
    if (gen_is_constant(ctx, node)) {
        long long val = gen_fold_constant(ctx, node);
        inst_mov_reg_imm(ctx->emit, REG_EAX, (int)val);
        return;
    }
    
    if (node->data.binary.op == BINOP_AND) {
        gen_short_circuit_and(ctx, node);
        return;
    }
    
    if (node->data.binary.op == BINOP_OR) {
        gen_short_circuit_or(ctx, node);
        return;
    }
    
    if (node->data.binary.op == BINOP_ADD) {
        gen_optimized_add(ctx, node);
        gen_expression(ctx, node->data.binary.right);
        inst_add_reg_reg(ctx->emit, REG_EAX, REG_EBX);
        return;
    }
    
    gen_expression(ctx, node->data.binary.left);
    inst_mov_reg_reg(ctx->emit, REG_EBX, REG_EAX);
    gen_expression(ctx, node->data.binary.right);
    
    switch (node->data.binary.op) {
        case BINOP_ADD:
            inst_add_reg_reg(ctx->emit, REG_EAX, REG_EBX);
            break;
        case BINOP_SUB:
            inst_sub_reg_reg(ctx->emit, REG_EAX, REG_EBX);
            break;
        case BINOP_MUL:
            inst_imul_reg_reg(ctx->emit, REG_EAX, REG_EBX);
            break;
        case BINOP_DIV:
            inst_cdq(ctx->emit);
            inst_idiv_reg(ctx->emit, REG_EBX);
            break;
        case BINOP_MOD:
            inst_cdq(ctx->emit);
            inst_idiv_reg(ctx->emit, REG_EBX);
            inst_mov_reg_reg(ctx->emit, REG_EAX, REG_EDX);
            break;
        case BINOP_BIT_AND:
            inst_and_reg_reg(ctx->emit, REG_EAX, REG_EBX);
            break;
        case BINOP_BIT_OR:
            inst_or_reg_reg(ctx->emit, REG_EAX, REG_EBX);
            break;
        case BINOP_BIT_XOR:
            inst_xor_reg_reg(ctx->emit, REG_EAX, REG_EBX);
            break;
        case BINOP_SHL:
            inst_shl_reg_imm(ctx->emit, REG_EAX, 1);
            break;
        case BINOP_SHR:
            inst_shr_reg_imm(ctx->emit, REG_EAX, 1);
            break;
        case BINOP_EQ:
            inst_cmp_reg_reg(ctx->emit, REG_EAX, REG_EBX);
            inst_sete_reg(ctx->emit, REG_EAX);
            inst_and_reg_imm(ctx->emit, REG_EAX, 0xFF);
            break;
        case BINOP_NE:
            inst_cmp_reg_reg(ctx->emit, REG_EAX, REG_EBX);
            inst_setne_reg(ctx->emit, REG_EAX);
            inst_and_reg_imm(ctx->emit, REG_EAX, 0xFF);
            break;
        case BINOP_LT:
            inst_cmp_reg_reg(ctx->emit, REG_EBX, REG_EAX);
            inst_setl_reg(ctx->emit, REG_EAX);
            inst_and_reg_imm(ctx->emit, REG_EAX, 0xFF);
            break;
        case BINOP_LE:
            inst_cmp_reg_reg(ctx->emit, REG_EBX, REG_EAX);
            inst_setle_reg(ctx->emit, REG_EAX);
            inst_and_reg_imm(ctx->emit, REG_EAX, 0xFF);
            break;
        case BINOP_GT:
            inst_cmp_reg_reg(ctx->emit, REG_EBX, REG_EAX);
            inst_setg_reg(ctx->emit, REG_EAX);
            inst_and_reg_imm(ctx->emit, REG_EAX, 0xFF);
            break;
        case BINOP_GE:
            inst_cmp_reg_reg(ctx->emit, REG_EBX, REG_EAX);
            inst_setge_reg(ctx->emit, REG_EAX);
            inst_and_reg_imm(ctx->emit, REG_EAX, 0xFF);
            break;
        default:
            break;
    }
}
