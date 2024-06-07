#include <string.h>

#include "s_http_serv.h"

s_buf s_handle_request(s_buf req) {
    char* buf = strdup(
            "HTTP/1.1 200 OK\nContent-Type: text/html\n\n<html><head><title>Hello World</title></head><h1>Hello World</h1></html>");

    s_buf res = {
            .buffer=buf,
            .size=strlen(buf),
            .cap=strlen(buf)
    };

    return res;
}