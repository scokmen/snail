#ifndef SNAIL_UNIX_H
#define SNAIL_UNIX_H

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
#  else
#    define SN_FORMAT(FROM)
#  endif
#  if __has_attribute (const)
#    define SN_CONST_FN __attribute__ ((const))
#  else
#    define SN_CONST_FN
#  endif
#  if __has_attribute (nonnull)
#    define SN_NONNULL(...) __attribute__((nonnull (__VA_ARGS__)))
#  else
#    define SN_NONNULL(...)
#  endif
#  if __has_attribute (warn_unused_result)
#    define SN_USE_RESULT __attribute__ ((warn_unused_result))
#  else
#    define SN_USE_RESULT
#  endif
#else
#  define SN_BUFFER
#  define SN_UNUSED
#  define SN_INLINE
#  define SN_FORMAT(FROM)
#  define SN_CONST_FN
#  define SN_NONNULL(...)
#  define SN_USE_RESULT
#endif

#endif //SNAIL_UNIX_H
