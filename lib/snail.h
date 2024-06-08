#ifndef SNAIL_SNAIL_H
#define SNAIL_SNAIL_H

#include <stddef.h>
#include "s_attributes.h"

/*
 * platform hard limits.
 */
#define S_HTTP_MAX_HEADER_SIZE  (4096)                        // 4KB for request headers.
#define S_HTTP_MAX_URI_SIZE     (1024)                        // 1KB for request URI.
#define S_HTTP_MAX_BODY_SIZE    (1024*1024)                   // 1MB for request body.
#define S_HTTP_MAX_REQUEST_SIZE (S_HTTP_MAX_HEADER_SIZE+S_HTTP_MAX_URI_SIZE+S_HTTP_MAX_BODY_SIZE+512)
#define S_HTTP_MAX_HEADER_COUNT ((int)(S_HTTP_MAX_HEADER_SIZE / 8)) // Num of maximum headers.

typedef struct s_http_header {
    const char *name;
    const char *value;
} s_http_header;

typedef struct s_http_headers {
    size_t size;
    size_t count;
    s_http_header *headers;
} s_http_headers;

typedef struct s_http_request {
    const int version;
    const char *path;
    const char *method;
    const s_http_headers headers;
} s_http_request;


#define LOG_ERR(MESSAGE) \
    fprintf(stderr, MESSAGE " %s:%d\n", __FILE__, __LINE__);

int s_listen(int port);

#endif //SNAIL_SNAIL_H
