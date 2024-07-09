#ifndef SNAIL_ERRNO_H
#define SNAIL_ERRNO_H

#define MALLOC_FAILED                   1
#define MIDDLEWARE_LIMIT_EXCEEDED       2

#define ERROR_CODE_MAP(XX)                                                   \
  XX(MALLOC_FAILED                      , "malloc() failed")                 \
  XX(MIDDLEWARE_LIMIT_EXCEEDED          , "middleware limit exceeded")       \

typedef enum {
  #define DEF_ERR_CODE_ENUM(code, _) SN_ERR_ ## code = code,
    ERROR_CODE_MAP(DEF_ERR_CODE_ENUM)
  #undef DEF_ERR_CODE_ENUM
} sn_err_t;

const char *sn_err_print(sn_err_t code);

#endif //SNAIL_ERRNO_H
