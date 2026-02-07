/**
 * @file linker.c
 * @brief Linking and assembly integration
 * @details Manages assembly file output and invokes the system assembler
 * and linker to produce executable binaries.
 */

#include <util/linker.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

/**
 * @brief Get system temporary directory path
 * @return Allocated string with temp directory, or NULL on failure
 */
char* linker_get_temp_dir(void)
{
    char* temp_dir = NULL;
    
#ifdef _WIN32
    char temp_path[MAX_PATH];
    DWORD result = GetTempPathA(MAX_PATH, temp_path);
    if (result > 0 && result < MAX_PATH) {
        temp_dir = malloc(strlen(temp_path) + 1);
        if (temp_dir) {
            strcpy(temp_dir, temp_path);
            size_t len = strlen(temp_dir);
            if (len > 0 && temp_dir[len - 1] == '\\') {
                temp_dir[len - 1] = '\0';
            }
        }
    }
#else
    const char* env_tmp = getenv("TMPDIR");
    if (!env_tmp) {
        env_tmp = getenv("TMP");
    }
    if (!env_tmp) {
        env_tmp = getenv("TEMP");
    }
    if (!env_tmp) {
        env_tmp = "/tmp";
    }
    
    temp_dir = malloc(strlen(env_tmp) + 1);
    if (temp_dir) {
        strcpy(temp_dir, env_tmp);
    }
#endif
    
    return temp_dir;
}

/**
 * @brief Create linker context
 * @param asm_file Assembly file path
 * @param output_file Output executable path
 * @return Allocated linker context
 */
linker_ctx* linker_create(const char* asm_file, const char* output_file)
{
    linker_ctx* ctx = malloc(sizeof(linker_ctx));
    if (!ctx)
        return NULL;

    ctx->temp_dir = linker_get_temp_dir();
    if (!ctx->temp_dir) {
        free(ctx);
        return NULL;
    }

    ctx->asm_file = malloc(strlen(asm_file) + 1);
    if (!ctx->asm_file) {
        free(ctx->temp_dir);
        free(ctx);
        return NULL;
    }
    strcpy(ctx->asm_file, asm_file);

    ctx->output_file = output_file;

    size_t obj_len = strlen(ctx->temp_dir) + strlen(asm_file) + 10;
    ctx->obj_file = malloc(obj_len);
    if (!ctx->obj_file) {
        free(ctx->asm_file);
        free(ctx->temp_dir);
        free(ctx);
        return NULL;
    }

    const char* basename = strrchr(asm_file, '/');
#ifdef _WIN32
    if (!basename) {
        basename = strrchr(asm_file, '\\');
    }
#endif
    if (!basename) {
        basename = asm_file;
    } else {
        basename++;
    }

    size_t basename_len = strlen(basename);
    snprintf(ctx->obj_file, obj_len, "%s/%s", ctx->temp_dir, basename);
    if (basename_len > 2 && strcmp(basename + basename_len - 2, ".s") == 0) {
        snprintf(ctx->obj_file, obj_len, "%s/%.*s.o", ctx->temp_dir, (int)(basename_len - 2), basename);
    } else {
        snprintf(ctx->obj_file, obj_len, "%s/%s.o", ctx->temp_dir, basename);
    }

    return ctx;
}

void linker_free(linker_ctx* ctx)
{
    if (ctx) {
        free(ctx->asm_file);
        free(ctx->temp_dir);
        free(ctx->obj_file);
        free(ctx);
    }
}

int linker_assemble_and_link(linker_ctx* ctx)
{
    char assemble_cmd[2048];

#ifdef _WIN32
    snprintf(assemble_cmd, sizeof(assemble_cmd), "as --32 \"%s\" -o \"%s\"", ctx->asm_file, ctx->obj_file);
#else
    snprintf(assemble_cmd, sizeof(assemble_cmd), "as --32 %s -o %s", ctx->asm_file, ctx->obj_file);
#endif

    if (system(assemble_cmd) != 0) {
        fprintf(stderr, "error: assembly failed\n");
        return 1;
    }

    char link_cmd[2048];
#ifdef _WIN32
    snprintf(link_cmd, sizeof(link_cmd), "ld -m i386pe -e main -o \"%s\" \"%s\"", ctx->output_file, ctx->obj_file);
#elif __APPLE__
    snprintf(link_cmd, sizeof(link_cmd), "ld -e main -o %s %s -lSystem", ctx->output_file, ctx->obj_file);
#else
    snprintf(link_cmd, sizeof(link_cmd), "ld -m elf_i386 -e main -o %s %s", ctx->output_file, ctx->obj_file);
#endif

    if (system(link_cmd) != 0) {
        fprintf(stderr, "error: linking failed\n");
        unlink(ctx->obj_file);
        return 1;
    }

    unlink(ctx->obj_file);
    return 0;
}
