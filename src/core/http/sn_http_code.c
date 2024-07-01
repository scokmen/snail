#include "sn_http_code.h"


#define SN_HTTP_ENUM_GENERATOR(name, msg) case HTTP_ ## name: return msg;

const char *sn_http_get_description(sn_http_code_t code) {
    switch (code) {
        SN_HTTP_CODE_MAP(SN_HTTP_ENUM_GENERATOR)
        default:
            return "";
    }
}

#undef SN_HTTP_ENUM_GENERATOR
