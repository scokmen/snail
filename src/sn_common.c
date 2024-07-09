#include "sn_common.h"

SN_CONST_FN
unsigned long djb2_hash(unsigned char *str) {
    int c;
    unsigned long hash = 5381;

    while ((c = *str++)) {
        hash = ((hash << 5) + hash) + c;
    }

    return hash;
}