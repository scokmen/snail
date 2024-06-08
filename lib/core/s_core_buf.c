#include <stdlib.h>
#include <string.h>
#include "s_core_buf.h"

s_buf s_buf_init(char *const buf, size_t size, size_t cap) {
    s_buf buffer = {
            .buffer = buf,
            .size = size,
            .cap = cap,
    };
    return buffer;
}

int s_buf_extend(s_buf *dst, const char *src, size_t size) {
    if (src == NULL) {
        return S_BUF_SRC_NULL;
    }
    if (dst == NULL || dst->buffer == NULL) {
        return S_BUF_DST_NULL;
    }
    if ((dst->size + size) > dst->cap) {
        return S_BUF_OVERLOAD;
    }

    void *ext = realloc(dst->buffer, dst->size + size);

    if (ext == NULL) {
        return S_BUF_MEM_ERR;
    }

    dst->buffer = ext;
    memcpy(dst->buffer + dst->size, src, size);
    dst->size = dst->size + size;

    return 0;
}
