#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <signal.h>
#include <snail.h>
#include <time.h>
#include "helpers/test_helpers.h"
#include "runner/test_runner.h"

tr_test_result create_socket() {
    socket_handler_t sock_fd = th_connect_server("0.0.0.0", "3000", 5, 1);

    if (sock_fd < 0) {
        return tr_fail("Cannot create socket.\n");
    }

    socket_handler_t *args = malloc(sizeof(socket_handler_t));
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

    close(*((socket_handler_t *) args));

    return tr_success("Socket was closed.\n");
}

tr_test_result send_request_assert_response(socket_handler_t sock_fd, const char *request, http_code_t expected_http_code) {
    http_code_t actual_http_code;
    ssize_t byte_sent = 0, total_byte_sent = 0;
    size_t request_size = 0;
    request_size = (size_t) strlen(request);

    while (1) {
        if (total_byte_sent >= request_size) {
            break;
        }
        byte_sent = send(sock_fd, request + total_byte_sent, request_size - total_byte_sent, 0);
        if (byte_sent < 0) {
            return tr_fail("Cannot send data.\n");
        }
        total_byte_sent += byte_sent;
    }

    actual_http_code = th_read_http_code(sock_fd);

    if (actual_http_code == expected_http_code) {
        return tr_success("Expected %d, Found: %d\n", expected_http_code, actual_http_code);
    }

    return tr_fail("Expected %d, Found: %d\n", expected_http_code, actual_http_code);
}

tr_test_result valid_http_request(const void *args) {
    char *request = "GET /index.html HTTP/1.1\r\nHost:localhost\r\n\r\n";
    return send_request_assert_response(*((socket_handler_t *) args), request, 200);
}

tr_test_result invalid_http_version(const void *args) {
    char *request = "GET /index.html HTTP/1.3\r\nHost:localhost\r\n\r\n";
    return send_request_assert_response(*((socket_handler_t *) args), request, 505);
}

tr_test_result too_long_uri(const void *args) {
    char *request;
    asprintf(&request, "GET /%s HTTP/1.1\r\nHost:localhost\r\n\r\n", generate_rand_str(1025));
    return send_request_assert_response(*((socket_handler_t *) args), request, 414);
}

tr_test_result too_large_header_single(const void *args) {
    char *request;
    asprintf(&request, "GET /path HTTP/1.1\r\nHeader-Name:%s\r\n\r\n", generate_rand_str(1015));
    return send_request_assert_response(*((socket_handler_t *) args), request, 431);
}

tr_test_result too_large_header_total(const void *args) {
    char *request;
    char *headers[4];

    for (int i = 0; i < 4; i++) {
        asprintf(&headers[i], "Header-%d: %s", i + 1, generate_rand_str(1016));
    }

    asprintf(&request, "GET /path HTTP/1.1\r\nHost:localhost\r\n%s\r\n%s\r\n%s\r\n%s\r\n\r\n",
             headers[0], headers[1], headers[2], headers[3]);

    return send_request_assert_response(*((socket_handler_t *) args), request, 431);
}

int run_tests() {
    tr_test_suit *suit = tr_new_suit("HTTP PARSE SUIT");

    tr_add_test_case(suit,
                     tr_new_case("Valid HTTP Request", create_socket, valid_http_request, close_socket));
    tr_add_test_case(suit,
                     tr_new_case("Invalid HTTP Version", create_socket, invalid_http_version, close_socket));
    tr_add_test_case(suit,
                     tr_new_case("Too Long URI", create_socket, too_long_uri, close_socket));
    tr_add_test_case(suit,
                     tr_new_case("Too Large Header (Single)", create_socket, too_large_header_single, close_socket));
    tr_add_test_case(suit,
                     tr_new_case("Too Large Header (Total)", create_socket, too_large_header_total, close_socket));

    return tr_run_suit(suit);
}

int main(int argc, char **argv) {
    srand(time(NULL));
    pid_t pid = fork();

    if (pid < 0) {
        fprintf(stderr, "Cannot fork process.\n");
        return EXIT_FAILURE;
    }

    if (pid == 0) {
        sn_listen(3000);
        return EXIT_SUCCESS;
    }

    int status = run_tests();

    kill(pid, SIGTERM);

    return status == true ? EXIT_SUCCESS : EXIT_FAILURE;
}