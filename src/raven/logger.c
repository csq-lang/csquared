/**
 * @file logger.c
 * @brief Logging system implementation
 * @details Provides formatted logging with severity levels, timestamps,
 * optional ANSI color support, and debug mode functionality.
 */

#include <csquare/logger.h>
#include <stdarg.h>
#include <stdio.h>
#include <time.h>

/**
 * @brief Main logging function
 * @details Outputs formatted log messages with timestamp and severity level.
 * @param level Log severity level
 * @param file Source file name (for debug mode)
 * @param line Source line number (for debug mode)
 * @param fmt Printf-style format string
 * @param ... Format arguments
 */
int csq_debug_enabled = 0;

void csq_log(LogLevel level, const char *file, int line, const char *fmt, ...) {

  if (level < RVN_LOG_MIN_LEVEL) {
    return;
  }

  static const char *level_strings[] = {"TRACE", "DEBUG", "INFO",
                                        "WARN",  "ERROR", "FATAL"};

#if RVN_LOG_ENABLE_COLORS
  static const char *level_colors[] = {"\x1b[94m", "\x1b[36m", "\x1b[32m",
                                       "\x1b[33m", "\x1b[31m", "\x1b[35m"};
  static const char *reset_color = "\x1b[0m";
  static const char *dim_color = "\x1b[90m";
#else
  static const char *level_colors[] = {"", "", "", "", "", ""};
  static const char *reset_color = "";
  static const char *dim_color = "";
#endif

  time_t t = time(NULL);
  struct tm *tm_info = localtime(&t);
  char time_buf[16];
  strftime(time_buf, sizeof(time_buf), "%H:%M:%S", tm_info);

#if RVN_DEBUG_MODE
  fprintf(stderr, "%s %s%-5s%s %s[%s:%d]%s ", time_buf, level_colors[level],
          level_strings[level], reset_color, dim_color, file, line,
          reset_color);
#else
  (void)file;
  (void)line;
  (void)dim_color;
  fprintf(stderr, "%s%-5s%s ", level_colors[level], level_strings[level],
          reset_color);
#endif

  va_list args;
  va_start(args, fmt);
  vfprintf(stderr, fmt, args);
  va_end(args);

  fprintf(stderr, "\n");
}
