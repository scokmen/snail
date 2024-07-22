#ifndef SNAIL_SN_COMMON_H
#define SNAIL_SN_COMMON_H

#include <stdlib.h>
#include "snail.h"

#define MALLOC_OR_RETURN_NULL(PTR, TYPE, COUNT) PTR = malloc(sizeof (TYPE) * COUNT);         \
                                                if (PTR == NULL) {                           \
                                                    return NULL;                             \
                                                }                                            \

#define MALLOC_OR_RETURN_ERR(PTR, TYPE, COUNT)  PTR = malloc(sizeof (TYPE) * COUNT);         \
                                                if (PTR == NULL) {                           \
                                                    return SN_ENOMEM;                        \
                                                }                                            \

SN_CONST_FN
unsigned long djb2_hash(unsigned char *str);

#endif //SNAIL_SN_COMMON_H
