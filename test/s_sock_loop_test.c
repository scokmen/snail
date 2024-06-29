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

tr_test_result create_socket() {
    socket_handler_t sock_fd = th_connect_server("0.0.0.0", "3000", 5, 1);

    if (sock_fd < 0) {
        return tr_fail("Cannot create socket.\n");
    }

    socket_handler_t *args = malloc(sizeof (socket_handler_t));
    if (args == NULL) {
        return tr_fail("Cannot allocate memory.\n");
    }
    *args = sock_fd;

    return tr_success_ext(args, "Socket created.\n");
}

tr_test_result close_socket(const void *args) {
    if (args == NULL) {
        return tr_success("Socket is empty.\n");
    }

    socket_handler_t sock_fd = *((socket_handler_t*)args);
    close(sock_fd);

    return tr_success("Socket was closed.\n");
}

tr_test_result minimal_http_request_case(const void *args) {
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
            return tr_fail("Cannot send data.\n");
        }
        total_byte_sent += byte_sent;
    }

    http_code = th_read_http_code(sock_fd);

    if (http_code == 200) {
        return tr_success("Http:200");
    }

    return tr_fail("Expected: 200, Found: %d", http_code);
}

int run_tests() {
    tr_test_suit *suit = tr_new_suit("HTTP PARSE SUIT");

    tr_add_test_case(suit,
                     tr_new_case("Minimal HTTP Request", create_socket, minimal_http_request_case, close_socket));

    return tr_run_suit(suit);
}

int main() {
    pid_t pid = fork();

    if (pid < 0) {
        fprintf(stderr, "Cannot fork process!");
        return EXIT_FAILURE;
    }

    if (pid == 0) {
        sn_listen(3000);
        return EXIT_SUCCESS;
    }

    int status = run_tests();

    kill(pid, SIGTERM);

    return status == 1 ? EXIT_SUCCESS : EXIT_FAILURE;
}