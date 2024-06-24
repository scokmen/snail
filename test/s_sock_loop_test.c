#include <snail.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <signal.h>
#include "runner/test_runner.h"
#include "helpers/test_helpers.h"

test_result create_socket() {
    socket_handler_t sock_fd = try_connect_server("0.0.0.0", "3000", 5, 1);

    if (sock_fd < 0) {
        return test_result_new(false, "Cannot create socket!");
    }

    socket_handler_t *args = malloc(sizeof (socket_handler_t));
    if (args == NULL) {
        return test_result_new(false, "Cannot allocate memory!");
    }
    *args = sock_fd;

    return test_result_with_arg(true, args, "Socket created!");
}

test_result close_socket(const void *args) {
    if (args == NULL) {
        return test_result_new(true, "Socket is empty!");
    }

    socket_handler_t sock_fd = *((socket_handler_t*)args);
    close(sock_fd);

    return test_result_new(true, "Socket was closed!");
}

test_result minimal_http_request_case(const void *args) {
    int http_code;
    ssize_t byte_sent = 0, total_byte_sent = 0;
    size_t request_size = 0;
    socket_handler_t sock_fd = *((socket_handler_t*)args);

    char *http_request = "GET /index.html HTTP/1.1\r\nHost:localhost\r\nContent-Type:application/json\r\n\r\n";
    request_size = (size_t)strlen(http_request);

    while(1) {
        if (total_byte_sent >= request_size) {
            break;
        }
        int buf_size = (5 < (request_size - total_byte_sent)) ? 5 : (request_size - total_byte_sent);
        byte_sent = send(sock_fd, http_request + total_byte_sent, buf_size, 0);
        if (byte_sent < 0) {
            return test_result_new(false, "Cannot send data!");
        }
        total_byte_sent += byte_sent;
    }

    http_code = read_socket(sock_fd);

    if (http_code == 200) {
        return test_result_new(true, "SUCCESS");
    }

    return test_result_new(false, "Expected: 200, Found: %d", http_code);
}

int run_tests() {
    test_suit *suit = test_suit_new("HTTP PARSE SUIT");

    test_suit_add(suit,
                  test_case_new("Minimal HTTP Request", create_socket, minimal_http_request_case, close_socket));

    return test_suit_run(suit);
}

int main() {
    pid_t pid = fork();

    if (pid < 0) {
        fprintf(stderr, "Cannot fork process!");
        return EXIT_FAILURE;
    }

    if (pid == 0) {
        s_listen(3000);
        return EXIT_SUCCESS;
    }

    int status = run_tests();

    kill(pid, SIGTERM);

    return status == 1 ? EXIT_SUCCESS : EXIT_FAILURE;
}