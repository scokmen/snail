#ifndef SNAIL_SNAIL_H
#define SNAIL_SNAIL_H


#if defined __has_attribute
#  if __has_attribute (nonstring)
#    define SN_BUFFER __attribute__ ((nonstring))
#  else
#    define SN_BUFFER
#  endif
#  if __has_attribute (unused)
#    define SN_UNUSED __attribute__ ((unused))
#  else
#    define SN_UNUSED
#  endif
#  if __has_attribute (always_inline)
#    define SN_INLINE __attribute__ ((always_inline))
#  else
#    define SN_INLINE
#  endif
#endif


/*
 * Platform hard limits.
 */
#define S_HTTP_MAX_HEADER_SIZE  (4096)                                // 4KB for request headers.
#define S_HTTP_MAX_URI_SIZE     (1024)                                // 1KB for request URI.
#define S_HTTP_MAX_BODY_SIZE    (1024 * 1024)                         // 1MB for request body.
#define S_HTTP_MAX_HEADER_COUNT ((int)(S_HTTP_MAX_HEADER_SIZE / 8))   // Num of maximum headers.

int s_listen(int port);

#endif //SNAIL_SNAIL_H
