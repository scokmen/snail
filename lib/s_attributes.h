#ifndef SNAIL_S_ATTRIBUTES_H
#define SNAIL_S_ATTRIBUTES_H

#ifndef SN_BUFFER
#if defined(__GNUC__)
#define SN_BUFFER __attribute__ ((nonstring))
#else
#define SN_BUFFER
#endif
#endif

#ifndef SN_UNUSED
#if defined(__GNUC__)
#define SN_UNUSED __attribute__((unused))
#else
#define SN_UNUSED
#endif
#endif

#ifndef SN_INLINE
#if defined(__GNUC__)
#define SN_INLINE __attribute__((always_inline))
#else
#define SN_INLINE
#endif
#endif

#endif //SNAIL_S_ATTRIBUTES_H
