#ifndef SNAIL_S_CORE_BUF_H
#define SNAIL_S_CORE_BUF_H

#include <stddef.h>

#include "../snail.h"

#define S_BUF_OVERLOAD (-1)
#define S_BUF_MEM_ERR  (-2)
#define S_BUF_SRC_NULL (-3)
#define S_BUF_DST_NULL (-4)

typedef struct s_buf {
    size_t size;
    size_t cap;
    char *buffer S_BUF_AREA;
} s_buf;

s_buf s_buf_init(char *buf, size_t size, size_t cap);

int s_buf_extend(s_buf *dst, const char *src, size_t size);

#endif //SNAIL_S_CORE_BUF_H
