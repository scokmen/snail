#ifndef SNAIL_S_LOGGER_H
#define SNAIL_S_LOGGER_H


#include "../snail.h"

#define SN_LOG_LEVEL_DEBUG (1)
#define SN_LOG_LEVEL_INFO  (2)
#define SN_LOG_LEVEL_WARN  (3)
#define SN_LOG_LEVEL_ERROR (4)
#define SN_LOG_LEVEL_FATAL (5)

void sn_log_set_level(int level);

void sn_log_debug(char *fmt, ...) SN_FORMAT(1);

void sn_log_info(char *fmt, ...) SN_FORMAT(1);

void sn_log_warn(char *fmt, ...) SN_FORMAT(1);

void sn_log_err(char *fmt, ...) SN_FORMAT(1);

void sn_log_fatal(char *fmt, ...) SN_FORMAT(1);

#endif
