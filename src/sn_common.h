#ifndef SNAIL_SN_COMMON_H
#define SNAIL_SN_COMMON_H

#define MALLOC_OR_RETURN_NULL(PTR, TYPE) PTR = malloc(sizeof (TYPE)); \
                                         if (PTR == NULL) {           \
                                             return NULL;             \
                                         }                            \

#endif //SNAIL_SN_COMMON_H
