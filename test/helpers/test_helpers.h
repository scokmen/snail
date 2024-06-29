#ifndef SNAIL_TEST_HELPERS_H
#define SNAIL_TEST_HELPERS_H

typedef int http_code_t;
typedef int socket_handler_t;

http_code_t th_read_http_code(socket_handler_t sock_fd);

socket_handler_t th_connect_server(const char *host, const char *port, int max_attempt, unsigned int backoff);

#endif //SNAIL_TEST_HELPERS_H
