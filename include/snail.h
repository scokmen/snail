#ifndef SNAIL_SNAIL_H
#define SNAIL_SNAIL_H

#if defined(__GCC__) || defined(__clang__)
    #include "headers/unix.h"
#else
    #include "headers/win.h"
#endif

#include "headers/map.h"
#include "headers/http.h"

int sn_listen(int port);

#endif //SNAIL_SNAIL_H
