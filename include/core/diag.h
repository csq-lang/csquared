/**
 * @file diag.h
 * @brief Diagnostic and error reporting system for the C² compiler.
 * @details Provides structures and functions for collecting, storing, and reporting compilation errors
 * with source location information, error categories, and formatted diagnostic output.
 */

#ifndef CORE_DIAG_H
#define CORE_DIAG_H

#include <stddef.h>
#include <stdbool.h>

/**
 * @brief Error type enumeration.
 * @details Categorizes different types of compilation errors that can be reported,
 * ranging from lexical errors to semantic and type-checking errors.
 */
typedef enum {
    DIAG_ERROR_INVALID_CHAR,              /**< Unrecognized character in source */
    DIAG_ERROR_UNTERMINATED_STRING,       /**< String literal missing closing quote */
    DIAG_ERROR_INVALID_ESCAPE,            /**< Invalid escape sequence in string */
    DIAG_ERROR_MALFORMED_NUMBER,          /**< Invalid number literal syntax */
    DIAG_ERROR_INVALID_BASE,              /**< Invalid numeric base specifier */
    DIAG_ERROR_UNRECOGNIZED_TOKEN,        /**< Unknown token encountered */
    DIAG_ERROR_UNEXPECTED_EOF,            /**< Unexpected end of file */
    DIAG_ERROR_INVALID_IDENTIFIER,        /**< Invalid identifier syntax */
    DIAG_ERROR_UNDEFINED_IDENTIFIER,      /**< Identifier not declared in scope */
    DIAG_ERROR_UNDEFINED_VARIABLE,        /**< Variable used but not defined */
    DIAG_ERROR_UNDEFINED_TYPE,            /**< Type not defined */
    DIAG_ERROR_UNDEFINED_FUNCTION,        /**< Function not declared */
    DIAG_ERROR_DUPLICATE_VARIABLE,        /**< Variable already defined in scope */
    DIAG_ERROR_DUPLICATE_FUNCTION,        /**< Function already defined */
    DIAG_ERROR_TYPE_MISMATCH,             /**< Type incompatibility in operation */
    DIAG_ERROR_INVALID_TYPE,              /**< Invalid type specification */
    DIAG_ERROR_INVALID_ARGUMENT,          /**< Invalid function argument */
    DIAG_ERROR_TOO_MANY_ARGS,             /**< Function called with excess arguments */
    DIAG_ERROR_TOO_FEW_ARGS,              /**< Function called with insufficient arguments */
    DIAG_ERROR_INVALID_OPERATION,         /**< Operation not valid for operand types */
    DIAG_ERROR_DIVIDE_BY_ZERO,            /**< Division by zero attempted */
    DIAG_ERROR_STACK_OVERFLOW,            /**< Stack overflow condition */
    DIAG_ERROR_NULL_POINTER,              /**< Null pointer dereference */
    DIAG_ERROR_OUT_OF_BOUNDS,             /**< Out of bounds access */
    DIAG_ERROR_ARRAY_INDEX_OUT_OF_BOUNDS, /**< Array index beyond bounds */
    DIAG_ERROR_ARRAY_INDEX_NOT_INTEGER,   /**< Non-integer array index */
    DIAG_ERROR_ARRAY_TYPE_MISMATCH,       /**< Array element type mismatch */
    DIAG_ERROR_ARRAY_EMPTY                /**< Array is empty */
} DiagErrorType;

/**
 * @brief Individual diagnostic error information.
 * @details Contains complete information about a single compilation error including
 * source location, error type, and descriptive message.
 */
typedef struct {
    const char* path;           /**< Source file path where error occurred */
    size_t line;                /**< Line number in source file */
    size_t column;              /**< Column number in source file */
    size_t length;              /**< Length of problematic token/code */
    DiagErrorType type;         /**< Category of the error */
    const char* message;        /**< Human-readable error message */
} DiagError;

/**
 * @brief Linked list node for diagnostic errors.
 * @details Single node in a linked list of diagnostic errors collected during compilation.
 */
typedef struct DiagNode {
    DiagError error;                  /**< Error information for this node */
    struct DiagNode* next;            /**< Pointer to next error node */
} DiagNode;

/**
 * @brief Diagnostic error collection and reporting context.
 * @details Accumulates all errors encountered during compilation with statistics.
 * Supports iteration and querying of collected errors.
 */
typedef struct {
    DiagNode* head;                   /**< First error in linked list */
    DiagNode* tail;                   /**< Last error in linked list */
    size_t count;                     /**< Total number of diagnostics */
    size_t error_count;               /**< Number of actual errors (vs warnings) */
} DiagReporter;

/**
 * @brief Creates a new diagnostic reporter instance.
 * @details Allocates and initializes an empty diagnostic reporter.
 * @return Pointer to newly allocated DiagReporter, or NULL on memory error.
 */
DiagReporter* diag_reporter_create(void);

/**
 * @brief Frees a diagnostic reporter and all collected errors.
 * @details Releases all memory associated with the reporter and its error list.
 * @param reporter The reporter to free. Safe to call with NULL.
 */
void diag_reporter_free(DiagReporter* reporter);

/**
 * @brief Reports a diagnostic error.
 * @details Records an error with source location and message without source context.
 * @param reporter The reporter instance.
 * @param type The error type category.
 * @param path Source file path.
 * @param line Line number in source file.
 * @param column Column number in source file.
 * @param length Length of problematic code.
 * @param message Error message.
 */
void diag_report(DiagReporter* reporter, DiagErrorType type,
                 const char* path, size_t line, size_t column,
                 size_t length, const char* message);

/**
 * @brief Reports a diagnostic error with source code context.
 * @details Records an error with full information including the line of source code.
 * @param reporter The reporter instance.
 * @param type The error type category.
 * @param path Source file path.
 * @param line Line number in source file.
 * @param column Column number in source file.
 * @param length Length of problematic code.
 * @param source_line The actual source code line.
 * @param message Error message.
 */
void diag_report_error(DiagReporter* reporter, DiagErrorType type,
                       const char* path, size_t line, size_t column,
                       size_t length, const char* source_line,
                       const char* message);

/**
 * @brief Prints all collected diagnostics to standard output.
 * @details Formats and displays all errors with source code context.
 * @param reporter The reporter containing errors to display.
 * @param source_buffer The full source code buffer for context.
 */
void diag_print_all(const DiagReporter* reporter, const char* source_buffer);

/**
 * @brief Prints a single diagnostic error to standard output.
 * @details Formats and displays one error with source code context.
 * @param error The error to display.
 * @param source_buffer The full source code buffer for context.
 */
void diag_print_error(const DiagError* error, const char* source_buffer);

/**
 * @brief Converts error type to human-readable string.
 * @details Returns the string name of an error type for display purposes.
 * @param type The error type to convert.
 * @return Static string describing the error type.
 */
const char* diag_error_type_string(DiagErrorType type);

/**
 * @brief Checks if any errors were reported.
 * @details Determines whether the reporter contains error-level diagnostics.
 * @param reporter The reporter to check.
 * @return True if errors exist, false otherwise.
 */
bool diag_has_errors(const DiagReporter* reporter);

#endif