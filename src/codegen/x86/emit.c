/**
 * @file emit.c
 * @brief Assembly code emission
 * @details Handles output of assembly instructions to file or buffer,
 * with support for both AT&T and Intel syntax.
 */

#include <codegen/x86/emit.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <limits.h>
#include <stdint.h>
#include <ctype.h>

#define INITIAL_CAPACITY 4096

/**
 * @brief Create emitter outputting to file
 * @param file Output file stream
 * @return Allocated emit context
 */
emit_ctx* emit_create_file(FILE* file) {
    emit_ctx* ctx = malloc(sizeof(emit_ctx));
    if (!ctx) return NULL;
    
    ctx->file = file;
    ctx->buffer = NULL;
    ctx->buffer_size = 0;
    ctx->buffer_capacity = 0;
    ctx->use_buffer = false;
    ctx->indent_level = 0;
    ctx->syntax = ASM_SYNTAX_ATT;
    
    return ctx;
}

/**
 * @brief Create emitter outputting to buffer
 * @return Allocated emit context with internal buffer
 */
emit_ctx* emit_create_buffer(void) {
    emit_ctx* ctx = malloc(sizeof(emit_ctx));
    if (!ctx) return NULL;
    
    ctx->file = NULL;
    ctx->buffer = malloc(INITIAL_CAPACITY);
    if (!ctx->buffer) {
        free(ctx);
        return NULL;
    }
    
    ctx->buffer[0] = '\0';
    ctx->buffer_size = 0;
    ctx->buffer_capacity = INITIAL_CAPACITY;
    ctx->use_buffer = true;
    ctx->indent_level = 0;
    ctx->syntax = ASM_SYNTAX_ATT;
    
    return ctx;
}

/**
 * @brief Free emitter context
 * @param ctx Emitter context to deallocate
 */
void emit_free(emit_ctx* ctx) {
    if (!ctx) return;
    free(ctx->buffer);
    free(ctx);
}

/**
 * @brief Set assembly syntax (AT&T or Intel)
 * @param ctx Emitter context
 * @param syntax Assembly syntax to use
 */
void emit_set_syntax(emit_ctx* ctx, emit_syntax syntax) {
    if (ctx) ctx->syntax = syntax;
}

static void ensure_capacity(emit_ctx* ctx, size_t needed) {
    if (!ctx->use_buffer) return;
    
    if (ctx->buffer_size + needed >= ctx->buffer_capacity) {
        size_t new_capacity = ctx->buffer_capacity * 2;
        
        while (new_capacity < ctx->buffer_size + needed) {
            if (new_capacity > SIZE_MAX / 2) {
                new_capacity = SIZE_MAX;
                break;
            }
            new_capacity *= 2;
        }
        
        if (new_capacity < ctx->buffer_size + needed) {
            return;
        }
        
        char* new_buffer = realloc(ctx->buffer, new_capacity);
        if (new_buffer) {
            ctx->buffer = new_buffer;
            ctx->buffer_capacity = new_capacity;
        }
    }
}

static void write_indent(emit_ctx* ctx) {
    for (int i = 0; i < ctx->indent_level; i++) {
        if (ctx->use_buffer) {
            ensure_capacity(ctx, 4);
            size_t remaining = ctx->buffer_capacity - ctx->buffer_size;
            if (remaining >= 4) {
                strncat(ctx->buffer, "    ", remaining - 1);
                ctx->buffer_size += 4;
            }
        } else if (ctx->file) {
            fprintf(ctx->file, "    ");
        }
    }
}

void emit_indent(emit_ctx* ctx) {
    if (ctx) ctx->indent_level++;
}

void emit_dedent(emit_ctx* ctx) {
    if (ctx && ctx->indent_level > 0) ctx->indent_level--;
}

void emit_newline(emit_ctx* ctx) {
    if (!ctx) return;
    
    if (ctx->use_buffer) {
        ensure_capacity(ctx, 2);
        size_t remaining = ctx->buffer_capacity - ctx->buffer_size;
        if (remaining >= 1) {
            strncat(ctx->buffer, "\n", remaining - 1);
            ctx->buffer_size++;
        }
    } else if (ctx->file) {
        fprintf(ctx->file, "\n");
    }
}

void emit_raw(emit_ctx* ctx, const char* str) {
    if (!ctx || !str) return;
    
    char converted[512];
    const char* output_str = str;
    
    if (ctx->syntax == ASM_SYNTAX_INTEL) {
        char temp[512];
        strncpy(temp, str, sizeof(temp) - 1);
        temp[sizeof(temp) - 1] = '\0';
        
        char* p = temp;
        char* dst = converted;
        while (*p && dst - converted < (int)sizeof(converted) - 1) {
            if (*p == '$') {
                p++;
                continue;
            } else {
                *dst++ = *p++;
            }
        }
        *dst = '\0';
        output_str = converted;
    }
    
    size_t len = strlen(output_str);
    
    if (ctx->use_buffer) {
        ensure_capacity(ctx, len + 1);
        size_t remaining = ctx->buffer_capacity - ctx->buffer_size;
        if (remaining > len) {
            strncat(ctx->buffer, output_str, remaining - 1);
            ctx->buffer_size += len;
        }
    } else if (ctx->file) {
        fprintf(ctx->file, "%s", output_str);
    }
}

void emit_format(emit_ctx* ctx, const char* fmt, ...) {
    if (!ctx || !fmt) return;
    
    va_list args;
    va_start(args, fmt);
    
    if (ctx->use_buffer) {
        char temp[2048];
        vsnprintf(temp, sizeof(temp), fmt, args);
        size_t len = strlen(temp);
        ensure_capacity(ctx, len + 1);
        size_t remaining = ctx->buffer_capacity - ctx->buffer_size;
        if (remaining > len) {
            strncat(ctx->buffer, temp, remaining - 1);
            ctx->buffer_size += len;
        }
    } else if (ctx->file) {
        vfprintf(ctx->file, fmt, args);
    }
    
    va_end(args);
}

void emit_line(emit_ctx* ctx, const char* fmt, ...) {
    if (!ctx || !fmt) return;
    
    write_indent(ctx);
    
    va_list args;
    va_start(args, fmt);
    
    if (ctx->use_buffer) {
        char temp[2048];
        vsnprintf(temp, sizeof(temp), fmt, args);
        size_t len = strlen(temp);
        ensure_capacity(ctx, len + 2);
        size_t remaining = ctx->buffer_capacity - ctx->buffer_size;
        if (remaining > len + 1) {
            strncat(ctx->buffer, temp, remaining - 1);
            ctx->buffer_size += len;
            strncat(ctx->buffer, "\n", remaining - len - 1);
            ctx->buffer_size += 1;
        }
    } else if (ctx->file) {
        vfprintf(ctx->file, fmt, args);
        fprintf(ctx->file, "\n");
    }
    
    va_end(args);
}

void emit_label(emit_ctx* ctx, const char* label) {
    if (!ctx || !label) return;
    
    if (ctx->use_buffer) {
        size_t label_len = strlen(label);
        ensure_capacity(ctx, label_len + 3);
        size_t remaining = ctx->buffer_capacity - ctx->buffer_size;
        if (remaining > label_len + 2) {
            strncat(ctx->buffer, label, remaining - 1);
            ctx->buffer_size += label_len;
            strncat(ctx->buffer, ":\n", remaining - label_len - 1);
            ctx->buffer_size += 2;
        }
    } else if (ctx->file) {
        fprintf(ctx->file, "%s:\n", label);
    }
}

void emit_directive(emit_ctx* ctx, const char* directive) {
    emit_line(ctx, ".%s", directive);
}

static void strip_att_suffix(char* dest, const char* opcode, size_t size) {
    if (!opcode || !dest || size == 0) return;
    
    size_t len = strlen(opcode);
    
    if (len > 0 && (opcode[len-1] == 'l' || opcode[len-1] == 'w' || opcode[len-1] == 'b' || opcode[len-1] == 'q')) {
        char last_char = opcode[len-1];
        
        if (len > 1 && opcode[len-2] == last_char) {
            if (len - 2 < size) {
                strncpy(dest, opcode, len - 2);
                dest[len - 2] = '\0';
            }
        } else {
            if (len - 1 < size) {
                strncpy(dest, opcode, len - 1);
                dest[len - 1] = '\0';
            }
        }
    } else {
        strncpy(dest, opcode, size - 1);
        dest[size - 1] = '\0';
    }
}

static void convert_to_intel_syntax(char* intel_operands, const char* att_operands, size_t size) {
    if (!intel_operands || !att_operands || size == 0) return;
    
    char temp[512];
    strncpy(temp, att_operands, sizeof(temp) - 1);
    temp[sizeof(temp) - 1] = '\0';
    
    char operand1[256] = {0};
    char operand2[256] = {0};
    
    char* comma_pos = strchr(temp, ',');
    if (comma_pos) {
        int op1_len = comma_pos - temp;
        strncpy(operand1, temp, op1_len);
        operand1[op1_len] = '\0';
        strncpy(operand2, comma_pos + 1, sizeof(operand2) - 1);
        operand2[sizeof(operand2) - 1] = '\0';
        
        char trimmed1[256], trimmed2[256];
        char* p = operand1;
        while (*p && isspace((unsigned char)*p)) p++;
        strncpy(trimmed1, p, sizeof(trimmed1) - 1);
        trimmed1[sizeof(trimmed1) - 1] = '\0';
        p = trimmed1 + strlen(trimmed1) - 1;
        while (p >= trimmed1 && isspace((unsigned char)*p)) {
            *p = '\0';
            p--;
        }
        
        p = operand2;
        while (*p && isspace((unsigned char)*p)) p++;
        strncpy(trimmed2, p, sizeof(trimmed2) - 1);
        trimmed2[sizeof(trimmed2) - 1] = '\0';
        p = trimmed2 + strlen(trimmed2) - 1;
        while (p >= trimmed2 && isspace((unsigned char)*p)) {
            *p = '\0';
            p--;
        }
        
        char converted1[256], converted2[256];
        char* src = trimmed1;
        char* dst = converted1;
        while (*src && dst - converted1 < (int)sizeof(converted1) - 1) {
            if (*src == '$') {
                *dst++ = '0';
                *dst++ = 'x';
                src++;
            } else if (*src == '%') {
                src++;
                while (*src && *src != ',' && *src != ')' && !isspace((unsigned char)*src)) {
                    *dst++ = *src++;
                }
            } else if (*src == '(') {
                *dst++ = '[';
                src++;
            } else if (*src == ')') {
                *dst++ = ']';
                src++;
            } else {
                *dst++ = *src++;
            }
        }
        *dst = '\0';
        
        src = trimmed2;
        dst = converted2;
        while (*src && dst - converted2 < (int)sizeof(converted2) - 1) {
            if (*src == '$') {
                *dst++ = '0';
                *dst++ = 'x';
                src++;
            } else if (*src == '%') {
                src++;
                while (*src && *src != ',' && *src != ')' && !isspace((unsigned char)*src)) {
                    *dst++ = *src++;
                }
            } else if (*src == '(') {
                *dst++ = '[';
                src++;
            } else if (*src == ')') {
                *dst++ = ']';
                src++;
            } else {
                *dst++ = *src++;
            }
        }
        *dst = '\0';
        
        snprintf(intel_operands, size, "%s, %s", converted2, converted1);
    } else {
        char converted[256];
        char* src = temp;
        char* dst = converted;
        while (*src && dst - converted < (int)sizeof(converted) - 1) {
            if (*src == '$') {
                *dst++ = '0';
                *dst++ = 'x';
                src++;
            } else if (*src == '%') {
                src++;
                while (*src && *src != ',' && *src != ')' && !isspace((unsigned char)*src)) {
                    *dst++ = *src++;
                }
            } else if (*src == '(') {
                *dst++ = '[';
                src++;
            } else if (*src == ')') {
                *dst++ = ']';
                src++;
            } else {
                *dst++ = *src++;
            }
        }
        *dst = '\0';
        
        strncpy(intel_operands, converted, size - 1);
        intel_operands[size - 1] = '\0';
    }
}

void emit_instruction(emit_ctx* ctx, const char* opcode, const char* operands, ...) {
    if (!ctx || !opcode) return;
    
    write_indent(ctx);
    
    va_list args;
    va_start(args, operands);
    
    if (operands && operands[0]) {
        if (ctx->use_buffer) {
            char temp[512];
            vsnprintf(temp, sizeof(temp), operands, args);
            
            char att_ops[512];
            strncpy(att_ops, temp, sizeof(att_ops) - 1);
            att_ops[sizeof(att_ops) - 1] = '\0';
            
            if (ctx->syntax == ASM_SYNTAX_INTEL) {
                char intel_ops[512];
                convert_to_intel_syntax(intel_ops, att_ops, sizeof(intel_ops));
                
                char intel_opcode[64];
                strip_att_suffix(intel_opcode, opcode, sizeof(intel_opcode));
                
                size_t opcode_len = strlen(intel_opcode);
                size_t ops_len = strlen(intel_ops);
                ensure_capacity(ctx, opcode_len + ops_len + 3);
                size_t remaining = ctx->buffer_capacity - ctx->buffer_size;
                if (remaining > opcode_len + ops_len + 2) {
                    strncat(ctx->buffer, intel_opcode, remaining - 1);
                    ctx->buffer_size += opcode_len;
                    strncat(ctx->buffer, " ", remaining - opcode_len - 1);
                    ctx->buffer_size += 1;
                    strncat(ctx->buffer, intel_ops, remaining - opcode_len - 2);
                    ctx->buffer_size += ops_len;
                    strncat(ctx->buffer, "\n", remaining - opcode_len - ops_len - 2);
                    ctx->buffer_size += 1;
                }
            } else {
                size_t opcode_len = strlen(opcode);
                size_t temp_len = strlen(temp);
                ensure_capacity(ctx, opcode_len + temp_len + 3);
                size_t remaining = ctx->buffer_capacity - ctx->buffer_size;
                if (remaining > opcode_len + temp_len + 2) {
                    strncat(ctx->buffer, opcode, remaining - 1);
                    ctx->buffer_size += opcode_len;
                    strncat(ctx->buffer, " ", remaining - opcode_len - 1);
                    ctx->buffer_size += 1;
                    strncat(ctx->buffer, temp, remaining - opcode_len - 2);
                    ctx->buffer_size += temp_len;
                    strncat(ctx->buffer, "\n", remaining - opcode_len - temp_len - 2);
                    ctx->buffer_size += 1;
                }
            }
        } else if (ctx->file) {
            char temp[512];
            vsnprintf(temp, sizeof(temp), operands, args);
            
            if (ctx->syntax == ASM_SYNTAX_INTEL) {
                char intel_ops[512];
                convert_to_intel_syntax(intel_ops, temp, sizeof(intel_ops));
                
                char intel_opcode[64];
                strip_att_suffix(intel_opcode, opcode, sizeof(intel_opcode));
                
                fprintf(ctx->file, "%s %s\n", intel_opcode, intel_ops);
            } else {
                fprintf(ctx->file, "%s %s\n", opcode, temp);
            }
        }
    } else {
        if (ctx->use_buffer) {
            emit_format(ctx, "%s\n", opcode);
        } else if (ctx->file) {
            fprintf(ctx->file, "%s\n", opcode);
        }
    }
    
    va_end(args);
}

char* emit_get_buffer(emit_ctx* ctx) {
    if (!ctx || !ctx->use_buffer) return NULL;
    return ctx->buffer;
}

size_t emit_get_buffer_size(emit_ctx* ctx) {
    if (!ctx) return 0;
    return ctx->buffer_size;
}

void emit_write_to_file(emit_ctx* ctx, FILE* file) {
    if (!ctx || !file || !ctx->use_buffer || !ctx->buffer) return;
    fprintf(file, "%s", ctx->buffer);
}