/**
 * @file builtins.h
 * @brief Built-in function definitions and intrinsic handling for the C² compiler.
 * @details Provides interfaces for recognizing, querying, and generating code for
 * built-in functions and compiler intrinsics like syscall, putc, and inline assembly.
 */

#ifndef CORE_BUILTINS_H
#define CORE_BUILTINS_H

#include <stdbool.h>

/**
 * @brief Built-in function type enumeration.
 * @details Categorizes the different types of built-in functions supported by the compiler.
 */
typedef enum {
    BUILTIN_SYSCALL,   /**< System call intrinsic */
    BUILTIN_PUTC,      /**< Character output intrinsic */
    BUILTIN_ASM,       /**< Inline assembly intrinsic */
    BUILTIN_COUNT      /**< Total number of built-in types */
} builtin_type;

/**
 * @brief Information about a built-in function.
 * @details Metadata describing a built-in function including its name, type,
 * and argument count constraints.
 */
typedef struct {
    const char* name;       /**< Function name */
    builtin_type type;      /**< Type/category of built-in */
    int min_args;           /**< Minimum argument count */
    int max_args;           /**< Maximum argument count */
} builtin_info;

/**
 * @brief Checks if a name refers to a built-in function.
 * @details Determines whether a given function name is a recognized built-in intrinsic.
 * @param name The function name to check.
 * @return True if the name is a built-in, false otherwise.
 */
bool builtin_is_builtin(const char* name);

/**
 * @brief Retrieves metadata for a built-in function.
 * @details Returns information about argument counts and type for a named built-in.
 * @param name The built-in function name.
 * @return Pointer to builtin_info structure, or NULL if not a built-in.
 */
const builtin_info* builtin_get_info(const char* name);

/**
 * @brief Generates code for a built-in function call.
 * @details Emits target code for a built-in function invocation with given arguments.
 * @param gen_ctx Code generation context.
 * @param name The built-in function name.
 * @param args Array of argument nodes.
 * @param arg_count Number of arguments.
 */
void builtin_gen_call(void* gen_ctx, const char* name, void* args, int arg_count);

#endif 