#ifndef SNAIL_SNAIL_H
#define SNAIL_SNAIL_H

#if defined(__GCC__) || defined(__clang__)
    #include "internal/unix/sn_attr.h"
#else
    #include "internal/unix/sn_attr.h"
#endif

int sn_listen(int port);

#endif //SNAIL_SNAIL_H
