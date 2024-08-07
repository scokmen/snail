#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <netdb.h>
#include <unistd.h>
#include "test_helpers.h"

static int open_socket(const char *host, const char *port, int attempt) {
    int sock_fd, connect_err;
    struct addrinfo hints, *res;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if (getaddrinfo(host, port, &hints, &res) != 0) {
        return -1;
    }

    sock_fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);

    if (sock_fd < 0) {
        fprintf(stderr, "[ATTEMPT %d]: Cannot open the socket: %s\n", attempt, strerror(sock_fd));
        return -1;
    }

    connect_err = connect(sock_fd, res->ai_addr, res->ai_addrlen);
    if (connect_err < 0) {
        fprintf(stderr, "[ATTEMPT %d]: Cannot establish the connection: %s\n", attempt, strerror(connect_err));
        close(sock_fd);
        return -1;
    }

    return sock_fd;
}

static int parse_status_code(const char *http_response) {
    int handled, status = -1, minor = 1;
    handled = sscanf(http_response, "HTTP/1.%d %d", &minor, &status);
    if (handled < 2) {
        return -1;
    }
    return status;
}

const char *th_random_string(int length) {
    char *str = malloc((sizeof(char) * length) + 1);
    if (str == NULL) {
        fprintf(stderr, "Cannot allocate memory!\n");
        return NULL;
    }
    for (int i = 0; i < length; i++) {
        str[i] = (char) (rand() % 26 + 65);
    }
    str[length] = 0;
    return str;
}

int th_read_http_code(int sock_fd) {
    int http_code;
    char buffer[1024];
    char *response = NULL;
    ssize_t byte_received, total_received = 0;

    while (1) {
        byte_received = recv(sock_fd, buffer, sizeof (buffer), 0);
        if (byte_received < 0) {
            fprintf(stderr, "An error occurred while receiving data!\n");
            http_code = -1;
            break;
        }
        if (byte_received == 0) {
            http_code = -1;
            if (response != NULL) {
                http_code = parse_status_code(response);
            }
            break;
        }
        if (response == NULL) {
            response = malloc(byte_received);
            if (response == NULL) {
                fprintf(stderr, "Cannot allocate memory!\n");
                http_code = -1;
                break;
            }
        } else {
            void *ptr = realloc(response, total_received + byte_received);
            if (ptr == NULL) {
                fprintf(stderr, "Cannot allocate memory!\n");
                http_code = -1;
                break;
            }
            response = ptr;
        }

        memcpy(response + total_received, buffer, byte_received);
        total_received += byte_received;
    }

    if (response != NULL) {
        free(response);
    }

    return http_code;
}

int th_connect_server(const char *host, const char *port, int max_attempt, unsigned int backoff) {
    int sock_fd, attempt = 1;

    while ((sock_fd = open_socket(host, port, attempt)) == -1) {
        sleep(backoff);
        if (attempt >= max_attempt) {
            return -1;
        }
        attempt++;
    }

    return sock_fd;
}
