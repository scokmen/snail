#ifndef SNAIL_SN_ATTR_H
#define SNAIL_SN_ATTR_H

#ifdef __has_attribute
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
#  if __has_attribute (format)
#    define SN_FORMAT(FROM) __attribute__ ((format (printf, (FROM), (FROM + 1))))
#  endif
#else
#  define SN_BUFFER
#  define SN_UNUSED
#  define SN_INLINE
#  define SN_FORMAT
#endif

#endif //SNAIL_SN_ATTR_H
