/**
 * @file source.h
 * @brief Source file loading and management
 * @details Provides functionality to load source files from disk and
 * manage their contents in memory for processing by the compiler.
 */

#ifndef C2_SOURCE_H
#define C2_SOURCE_H

#include <stddef.h>

/**
 * @struct csq_source
 * @brief Represents a loaded source file
 * @details Contains the source code buffer, file path, and metadata
 * needed for compilation and error reporting.
 */
typedef struct {
    const char* path;       /**< Original file path provided by user */
    char* abs_path;         /**< Absolute path to the source file */
    char* buffer;           /**< Complete source code as string */
    size_t size;            /**< Size of buffer in bytes */
} csq_source;

/**
 * @brief Load a source file from disk
 * @param path Path to the source file (relative or absolute)
 * @return Pointer to csq_source structure, or NULL on error
 * @details Reads the entire file into memory. Returns NULL if file
 * cannot be opened or memory allocation fails.
 */
csq_source* source_load(const char* path);

/**
 * @brief Free memory allocated for a source file
 * @param source Pointer to csq_source structure to free
 * @details Must be called when source is no longer needed to prevent memory leaks.
 */
void source_free(csq_source* source);

#endif
