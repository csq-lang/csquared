/**
 * @file linker.h
 * @brief Linker and assembly integration for the C² compiler.
 * @details Provides interfaces for assembling generated code and linking object files
 * to produce executable binaries.
 */

#ifndef C2_UTIL_LINKER_H
#define C2_UTIL_LINKER_H

/**
 * @brief Linker context for assembly and linking operations.
 * @details Maintains state for the assembly and linking process including file paths.
 */
typedef struct {
    char* asm_file;             /**< Path to generated assembly file */
    const char* output_file;    /**< Path to output executable file */
    char* temp_dir;             /**< Temporary directory for intermediate files */
    char* obj_file;             /**< Path to generated object file */
} linker_ctx;

/**
 * @brief Creates a linker context.
 * @details Initializes linker state with input and output file paths.
 * @param asm_file Path to the generated assembly file.
 * @param output_file Path to the desired output executable.
 * @return Pointer to newly allocated linker context, or NULL on error.
 */
linker_ctx* linker_create(const char* asm_file, const char* output_file);

/**
 * @brief Frees a linker context.
 * @details Deallocates the linker context and cleans up temporary files.
 * @param ctx The linker context to free. Safe to call with NULL.
 */
void linker_free(linker_ctx* ctx);

/**
 * @brief Assembles and links the generated assembly.
 * @details Invokes assembler to convert assembly to object code and links to create executable.
 * @param ctx The linker context.
 * @return 0 on success, non-zero error code on failure.
 */
int linker_assemble_and_link(linker_ctx* ctx);

/**
 * @brief Gets the system temporary directory.
 * @details Returns the path to the system's temporary directory.
 * @return Pointer to temp directory path string (owned by library).
 */
char* linker_get_temp_dir(void);

#endif
