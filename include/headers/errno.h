#ifndef SNAIL_ERRNO_H
#define SNAIL_ERRNO_H

#include <errno.h>

#define ERROR_CODE_MAP(XX)                                                   \
  XX(ENOMEM                             , "not enough memory")               \

typedef enum {
  #define DEF_ERR_CODE_ENUM(code, _) SN_ ## code = code,
    ERROR_CODE_MAP(DEF_ERR_CODE_ENUM)
  #undef DEF_ERR_CODE_ENUM
} sn_err_t;

const char *sn_err_print(sn_err_t code);

#endif //SNAIL_ERRNO_H
