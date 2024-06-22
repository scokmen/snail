#include <snail.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netdb.h>
#include <signal.h>
#include <errno.h>

#define RECV_MAX_BUF (1024)

typedef int socket_handler_t;

socket_handler_t open_socket(int attempt, const char *host, const char *port) {
    int connect_err;
    struct addrinfo hints, *res;
    socket_handler_t sock_fd;
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if (getaddrinfo(host, port, &hints, &res) != 0) {
        return -1;
    }

    sock_fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);

    if (sock_fd < 0) {
        fprintf(stderr, "[ATTEMPT %d]: Cannot open the socket: %s", attempt, strerror(sock_fd));
        return -1;
    }

    connect_err = connect(sock_fd, res->ai_addr, res->ai_addrlen);
    if (connect_err < 0) {
        fprintf(stderr, "[ATTEMPT %d]: Cannot establish the connection: %s", attempt, strerror(connect_err));
        close(sock_fd);
        return -1;
    }

    return sock_fd;
}

socket_handler_t connect_server(const char *host, const char *port, int max_attempt, unsigned int backoff) {
    socket_handler_t sock_fd;
    int attempt = 1;

    while ((sock_fd = open_socket(attempt, host, port)) == -1) {
        sleep(backoff);
        if (attempt >= max_attempt) {
            return -1;
        }
        attempt++;
    }

    return sock_fd;
}

int parse_status_code(const char *http_response) {
    int handled, status = -1;
    handled = sscanf(http_response, "HTTP/1.1 %d", &status);
    if (handled < 1) {
        return -1;
    }
    return status;
}

int read_socket(socket_handler_t sock_fd) {
    char buffer[RECV_MAX_BUF];
    char *response;
    ssize_t byte_received, total_received = 0;

    while (1) {
        byte_received = recv(sock_fd, buffer, RECV_MAX_BUF, 0);
        if (byte_received < 0) {
            return -1;
        }
        if (byte_received == 0) {
            return parse_status_code(response);
        }
        if (response == NULL) {
            response = malloc(byte_received);
        } else {
            void *ext = realloc(response, total_received + byte_received);
            response = ext;
        }

        memcpy(response + total_received, buffer, byte_received);
        total_received += byte_received;
    }

    return -1;
}

int start_server(int port) {
    fprintf(stdout, "Server is listening port: %d", port);
    return s_listen(port);
}

int run_tests() {
    socket_handler_t sock_fd = connect_server("0.0.0.0", "3000", 5, 1);

    if (sock_fd < 0) {
        return -1;
    }

    int http_code;
    ssize_t byte_sent;
    char *header = "GET /index.html HTTP/1.1\r\n\r\n";

    byte_sent = send(sock_fd, header, strlen(header), 0);
    if (byte_sent < 0) {
        fprintf(stderr, "Cannot send data: %s", strerror(errno));
        return -1;
    }

    http_code = read_socket(sock_fd);

    return http_code == 200;
}

int main() {
    pid_t pid = fork();

    if (pid < 0) {
        fprintf(stderr, "Cannot fork process!");
        return EXIT_FAILURE;
    }

    if (pid == 0) {
        start_server(3000);
        return EXIT_SUCCESS;
    }

    int status = run_tests();
    printf("STATUS %d", status);
    kill(pid, SIGTERM);
    exit(status == 1 ? EXIT_SUCCESS : EXIT_FAILURE);
}