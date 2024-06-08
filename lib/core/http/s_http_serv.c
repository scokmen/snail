#include <string.h>
#include <picohttpparser.h>
#include "../snail.h"
#include "s_http_serv.h"

static void parse_request(s_buf req) {
    const char *method, *path;
    int req_size, http_minor_version;
    struct phr_header headers[S_HTTP_MAX_HEADER_COUNT];
    size_t header_length = S_HTTP_MAX_HEADER_COUNT;
    size_t method_length, path_length;

    req_size = phr_parse_request(req.buffer, req.size, &method, &method_length, &path, &path_length,
                                 &http_minor_version, headers, &header_length, 0);

    if (req_size < 0) {
        // HANDLE PARSE ERROR
    }

    // DISPATCH REQ.
}

s_buf s_handle_request(s_buf req) {
    parse_request(req);
    char* buf = strdup(
            "HTTP/1.1 200 OK\nContent-Type: text/html\n\n<html><head><title>Hello World</title></head><h1>Hello World</h1></html>");

    s_buf res = {
            .buffer=buf,
            .size=strlen(buf),
            .cap=strlen(buf)
    };

    return res;
}