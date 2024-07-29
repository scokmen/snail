#ifndef SNAIL_SNAIL_H
#define SNAIL_SNAIL_H

#if defined(__GCC__) || defined(__clang__)
    #include "headers/unix.h"
#else
    #include "headers/win.h"
#endif

#include "headers/errno.h"
#include "headers/types.h"
#include "headers/http.h"
#include "headers/router.h"

SN_NONNULL(2)
int sn_listen(int port, sn_route_group_t *route_group);

#endif //SNAIL_SNAIL_H
