#ifndef SNAIL_SNAIL_H
#define SNAIL_SNAIL_H

#include <stddef.h>
#include "s_attributes.h"

/*
 * Platform hard limits.
 */
#define S_HTTP_MAX_HEADER_SIZE  (4096)                                // 4KB for request headers.
#define S_HTTP_MAX_URI_SIZE     (1024)                                // 1KB for request URI.
#define S_HTTP_MAX_BODY_SIZE    (1024 * 1024)                         // 1MB for request body.
#define S_HTTP_MAX_HEADER_COUNT ((int)(S_HTTP_MAX_HEADER_SIZE / 8))   // Num of maximum headers.

int s_listen(int port);

#endif //SNAIL_SNAIL_H
