#ifndef SNAIL_TEST_HELPERS_H
#define SNAIL_TEST_HELPERS_H

const char *th_random_string(int length);

int th_read_http_code(int sock_fd);

int th_connect_server(const char *host, const char *port, int max_attempt, unsigned int backoff);

#endif //SNAIL_TEST_HELPERS_H
