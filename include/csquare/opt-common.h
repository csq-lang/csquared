/**
 * @file opt-common.h
 * @brief Command-line options and configuration for the C² compiler.
 * @details Defines structures and functions for parsing and managing compiler options,
 * including debug flags, optimization levels, and assembly backend selection.
 */

#ifndef C2_OPT_COMMON_H
#define C2_OPT_COMMON_H

/**
 * @brief Assembly backend type selection.
 * @details Specifies which assembly syntax and backend to use for code generation.
 */
typedef enum {
    ASM_BACKEND_NONE = 0,   /**< No assembly backend */
    ASM_BACKEND_AT_T = 1,    /**< AT&T assembly syntax */
    ASM_BACKEND_INTEL = 2    /**< Intel assembly syntax */
} asm_backend_t;

/**
 * @brief Compiler command-line options.
 * @details Stores all configurable options parsed from the command line,
 * including debug flags, assembly format, and file paths.
 */
typedef struct {
    int debug_lexer;                /**< Enable lexer debug output */
    int debug_ast;                  /**< Enable AST debug output */
    int emit_asm;                   /**< Emit assembly code */
    asm_backend_t asm_backend;      /**< Selected assembly backend syntax */
    char* output_file;              /**< Output file path */
    char* input_file;               /**< Input source file path */
} csq_options;

/**
 * @brief Parses command-line arguments into options structure.
 * @details Processes argc and argv to extract compiler options, flags, and file paths.
 * Returns a dynamically allocated options structure that must be freed with options_free().
 * @param argc The argument count from main().
 * @param argv The argument vector from main().
 * @return Pointer to a newly allocated csq_options structure, or NULL on error.
 */
csq_options* options_parse(int argc, char** argv);

/**
 * @brief Frees compiler options structure.
 * @details Releases memory allocated by options_parse(). Must be called for every
 * options structure created by options_parse().
 * @param opts The options structure to free. Safe to call with NULL.
 */
void options_free(csq_options* opts);

#endif
