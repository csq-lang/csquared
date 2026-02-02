#ifndef RAVEN_LOGGER_H
#define RAVEN_LOGGER_H

#include <raven/config.h>

typedef enum {
  LOG_TRACE,
  LOG_DEBUG,
  LOG_INFO,
  LOG_WARN,
  LOG_ERROR,
  LOG_FATAL
} LogLevel;

void raven_log(LogLevel level, const char *file, int line, const char *fmt,
               ...);

extern int raven_debug_enabled;

#define RVN_FATAL(...) raven_log(LOG_FATAL, __FILE__, __LINE__, __VA_ARGS__)
#define RVN_ERROR(...) raven_log(LOG_ERROR, __FILE__, __LINE__, __VA_ARGS__)
#define RVN_WARN(...) raven_log(LOG_WARN, __FILE__, __LINE__, __VA_ARGS__)
#define RVN_INFO(...)                                                          \
  do {                                                                         \
    if (raven_debug_enabled)                                                   \
      raven_log(LOG_INFO, __FILE__, __LINE__, __VA_ARGS__);                    \
  } while (0)

#define RVN_DEBUG_LOG(...)                                                     \
  do {                                                                         \
    if (raven_debug_enabled)                                                   \
      raven_log(LOG_DEBUG, __FILE__, __LINE__, __VA_ARGS__);                   \
  } while (0)
#define RVN_TRACE_LOG(...)                                                     \
  do {                                                                         \
    if (raven_debug_enabled)                                                   \
      raven_log(LOG_TRACE, __FILE__, __LINE__, __VA_ARGS__);                   \
  } while (0)

#endif
