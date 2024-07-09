#ifndef SNAIL_SN_COMMON_H
#define SNAIL_SN_COMMON_H

#include "snail.h"

#define MALLOC_OR_RETURN_NULL(PTR, TYPE) PTR = malloc(sizeof (TYPE));                 \
                                         if (PTR == NULL) {                           \
                                             return NULL;                             \
                                         }                                            \

#define MALLOC_OR_RETURN_ERR(PTR, TYPE)  PTR = malloc(sizeof (TYPE));                 \
                                         if (PTR == NULL) {                           \
                                             return MALLOC_FAILED;                    \
                                         }                                            \

SN_CONST_FN
unsigned long djb2_hash(unsigned char *str);

#endif //SNAIL_SN_COMMON_H
