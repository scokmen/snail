#include <stdio.h>
#include <stdarg.h>
#include "sn_logger.h"

static int min_log_level = LOG_LEVEL_DEBUG;

#define GET_OUTPUT_DEV(LEVEL) ((LEVEL < LOG_LEVEL_ERROR) ? stdout : stderr)

#define WRITE_LOG(LEVEL, FMT) if (min_log_level <= LEVEL) {                    \
                                  va_list args;                                \
                                  va_start(args, FMT);                         \
                                  vfprintf(GET_OUTPUT_DEV(LEVEL), fmt, args);  \
                                  va_end(args);                                \
                              }                                                \

void sn_log_level(int level) {
    if (level <= LOG_LEVEL_DEBUG) {
        min_log_level = LOG_LEVEL_DEBUG;
        return;
    }
    if (level >= LOG_LEVEL_FATAL) {
        min_log_level = LOG_LEVEL_FATAL;
        return;
    }
    min_log_level = level;
}

void sn_log_debug(char *fmt, ...) {
    WRITE_LOG(LOG_LEVEL_DEBUG, fmt)
}

void sn_log_info(char *fmt, ...) {
    WRITE_LOG(LOG_LEVEL_INFO, fmt)
}

void sn_log_warn(char *fmt, ...) {
    WRITE_LOG(LOG_LEVEL_WARN, fmt)
}

void sn_log_err(char *fmt, ...) {
    WRITE_LOG(LOG_LEVEL_ERROR, fmt)
}

void sn_log_fatal(char *fmt, ...) {
    WRITE_LOG(LOG_LEVEL_FATAL, fmt)
}