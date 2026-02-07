/**
 * @file logger.h
 * @brief Logging system for the C² compiler.
 * @details Provides logging functionality with multiple log levels and debug mode support.
 * Includes convenience macros for logging at different severity levels with file and line information.
 */

#ifndef C2_LOGGER_H
#define C2_LOGGER_H

#include <csquare/config.h>

/**
 * @brief Log level enumeration.
 * @details Defines the severity levels for logging output, from trace-level debug information
 * to fatal errors. Used to control log verbosity and filtering.
 */
typedef enum {
    LOG_TRACE,  /**< Trace-level debug information */
    LOG_DEBUG,  /**< Debug-level information */
    LOG_INFO,   /**< Informational messages */
    LOG_WARN,   /**< Warning messages */
    LOG_ERROR,  /**< Error messages */
    LOG_FATAL   /**< Fatal error messages */
} LogLevel;

/**
 * @brief Logs a message with the specified log level.
 * @details Internal logging function that outputs messages with automatic file and line number tracking.
 * Respects the debug mode configuration and minimum log level settings.
 * @param level The severity level of the log message.
 * @param file The source file where the log call originated (typically __FILE__).
 * @param line The line number in the source file (typically __LINE__).
 * @param fmt Printf-style format string for the log message.
 * @param ... Variable arguments corresponding to the format string.
 */
void csq_log(LogLevel level, const char* file, int line, const char* fmt, ...);

/**
 * @brief Global debug mode flag.
 * @details When set to non-zero, enables debug-level logging output.
 * Controls whether DEBUG and TRACE log messages are actually printed.
 */
extern int csq_debug_enabled;

/**
 * @brief Macro for logging fatal errors.
 * @details Always logs regardless of debug mode. Use for unrecoverable errors.
 * Automatically captures file and line information.
 */
#define RVN_FATAL(...) csq_log(LOG_FATAL, __FILE__, __LINE__, __VA_ARGS__)

/**
 * @brief Macro for logging errors.
 * @details Always logs regardless of debug mode. Use for serious issues.
 * Automatically captures file and line information.
 */
#define RVN_ERROR(...) csq_log(LOG_ERROR, __FILE__, __LINE__, __VA_ARGS__)

/**
 * @brief Macro for logging warnings.
 * @details Always logs regardless of debug mode. Use for potentially problematic conditions.
 * Automatically captures file and line information.
 */
#define RVN_WARN(...)  csq_log(LOG_WARN,  __FILE__, __LINE__, __VA_ARGS__)

/**
 * @brief Macro for logging informational messages.
 * @details Only logs when debug mode is enabled. Use for general information.
 * Automatically captures file and line information.
 */
#define RVN_INFO(...)  do { if (csq_debug_enabled) csq_log(LOG_INFO, __FILE__, __LINE__, __VA_ARGS__); } while(0)

/**
 * @brief Macro for debug-level logging.
 * @details Only logs when debug mode is enabled. Use for development debugging.
 * Automatically captures file and line information.
 */
#define RVN_DEBUG_LOG(...) do { if (csq_debug_enabled) csq_log(LOG_DEBUG, __FILE__, __LINE__, __VA_ARGS__); } while(0)

/**
 * @brief Macro for trace-level logging.
 * @details Only logs when debug mode is enabled. Use for detailed execution tracing.
 * Automatically captures file and line information.
 */
#define RVN_TRACE_LOG(...) do { if (csq_debug_enabled) csq_log(LOG_TRACE, __FILE__, __LINE__, __VA_ARGS__); } while(0)

#endif