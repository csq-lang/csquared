#ifndef RAVEN_CONFIG_H
#define RAVEN_CONFIG_H

#ifndef RVN_DEBUG_MODE
#if defined(DEBUG)
#define RVN_DEBUG_MODE 1
#else
#define RVN_DEBUG_MODE 0
#endif
#endif
#define RVN_LOG_ENABLE_COLORS 1
#if RVN_DEBUG_MODE
#define RVN_LOG_MIN_LEVEL 0
#else
#define RVN_LOG_MIN_LEVEL 2
#endif

#endif
