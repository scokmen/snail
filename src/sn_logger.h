#ifndef SNAIL_SN_LOGGER_H
#define SNAIL_SN_LOGGER_H

#include "snail.h"

#define LOG_LEVEL_DEBUG (1)
#define LOG_LEVEL_INFO  (2)
#define LOG_LEVEL_WARN  (3)
#define LOG_LEVEL_ERROR (4)
#define LOG_LEVEL_FATAL (5)

void sn_log_set_level(int level);

SN_FORMAT(1)
void sn_log_debug(char *fmt, ...);

SN_FORMAT(1)
void sn_log_info(char *fmt, ...);

SN_FORMAT(1)
void sn_log_warn(char *fmt, ...);

SN_FORMAT(1)
void sn_log_err(char *fmt, ...);

SN_FORMAT(1)
void sn_log_fatal(char *fmt, ...);

#endif
