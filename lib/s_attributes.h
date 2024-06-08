#ifndef SNAIL_S_ATTRIBUTES_H
#define SNAIL_S_ATTRIBUTES_H

#ifndef S_BUFFER
#if defined(__GNUC__)
#define S_BUF_AREA __attribute__ ((nonstring))
#else
#define S_BUF_AREA
#endif
#endif

#ifndef S_UNUSED
#if defined(__GNUC__)
#define S_UNUSED __attribute__((unused))
#else
#define S_NOT_USED
#endif
#endif

#endif //SNAIL_S_ATTRIBUTES_H
