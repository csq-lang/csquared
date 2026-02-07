/**
 * @file config.h
 * @brief Global configuration and compilation settings for the C² compiler.
 * @details Defines compile-time configuration macros, debug settings, and portability abstractions
 * for memory management and logging functionality.
 */

#ifndef C2_CONFIG_H
#define C2_CONFIG_H

#include <string.h>

/**
 * @def RVN_DEBUG_MODE
 * @brief Global debug mode flag.
 * @details When enabled (set to 1), activates debug logging and additional runtime checks.
 * Automatically set based on the DEBUG macro if not explicitly defined.
 */
#ifndef RVN_DEBUG_MODE
    #if defined(DEBUG)
        #define RVN_DEBUG_MODE 1
    #else
        #define RVN_DEBUG_MODE 0
    #endif
#endif

/**
 * @def RVN_LOG_ENABLE_COLORS
 * @brief Enables colored output in log messages.
 * @details When set to 1, log output includes ANSI color codes for better readability.
 */
#define RVN_LOG_ENABLE_COLORS 1

/**
 * @def RVN_LOG_MIN_LEVEL
 * @brief Minimum log level to output.
 * @details Messages below this level are suppressed. In debug mode, this is set to 0 (all messages).
 * In release mode, set to 2 to suppress trace and debug messages.
 */
#if RVN_DEBUG_MODE
    #define RVN_LOG_MIN_LEVEL 0  /* Log all levels in debug mode */
#else
    #define RVN_LOG_MIN_LEVEL 2  /* Only info and above in release mode */
#endif

/**
 * @def RVN_FREE
 * @brief Portability abstraction for memory deallocation.
 * @details Maps to platform-specific free function. Currently uses standard C free().
 */
#define RVN_FREE        free

/**
 * @def RVN_MALLOC
 * @brief Portability abstraction for memory allocation.
 * @details Maps to platform-specific malloc function. Currently uses standard C malloc().
 */
#define RVN_MALLOC      malloc

/**
 * @def RVN_MEMCPY
 * @brief Portability abstraction for memory copying.
 * @details Maps to platform-specific memcpy function. Currently uses standard C memcpy().
 */
#define RVN_MEMCPY      memcpy 


#endif