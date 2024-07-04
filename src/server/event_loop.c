#include "uv.h"
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include "picohttpparser.h"
#include "snail.h"
#include "sn_logger.h"

#define HTTP_MAX_TOTAL_HEADER_SIZE  (4096)                                   // 4KB for request headers.
#define HTTP_MAX_SINGLE_HEADER_SIZE (1024)                                   // 1KB for single request header.
#define HTTP_MAX_URI_SIZE           (1024)                                   // 1KB for request URI.
#define HTTP_MAX_HEADER_COUNT       ((int)(HTTP_MAX_TOTAL_HEADER_SIZE / 8))  // Num of maximum headers.
#define INC_SOCKET_BUF_SIZE         (65536)
#define TCP_MAX_CON_BACKLOG         (128)

typedef struct {
    const char *path;
    const char *method;
    char *request_buf SN_BUFFER;
    char *response_buf SN_BUFFER;
    size_t buf_length;
    size_t buf_cursor;
    size_t path_length;
    size_t method_length;
    size_t header_length;
    struct phr_header headers[HTTP_MAX_HEADER_COUNT];
} sock_http_data;

typedef struct {
    sock_http_data *http_data;
    uv_tcp_t *tcp_client;
    uv_work_t *work_req;
    uv_write_t *write_req;
} sock_loop_data;

/*
 * UV: uv_close_cb
 * https://docs.libuv.org/en/v1.x/handle.html#c.uv_close_cb
 */
static void on_close_callback(uv_handle_t *handle) {
    if (handle != NULL) {
        free(handle);
    }
}

static void sock_http_data_free(sock_http_data *loop_data) {
    if (loop_data == NULL) {
        return;
    }

    if (loop_data->request_buf != NULL) {
        free(loop_data->request_buf);
    }

    if (loop_data->response_buf != NULL) {
        free(loop_data->response_buf);
    }
}

static void sock_loop_data_free(sock_loop_data *loop_data) {
    if (loop_data->tcp_client != NULL && !uv_is_closing((uv_handle_t *) loop_data->tcp_client)) {
        uv_read_stop((uv_stream_t *) loop_data->tcp_client);
        uv_close((uv_handle_t *) loop_data->tcp_client, on_close_callback);
    }

    if (loop_data->work_req != NULL) {
        free(loop_data->work_req);
    }

    if (loop_data->write_req != NULL) {
        free(loop_data->write_req);
    }

    sock_http_data_free(loop_data->http_data);
    free(loop_data);
}

SN_INLINE
static inline void clear_loop_request(uv_stream_t *stream, const uv_buf_t *buf) {
    sock_loop_data_free((sock_loop_data *) stream->data);
    if (buf != NULL) {
        free(buf->base);
    }
}

/*
 * UV: uv_write_cb
 * https://docs.libuv.org/en/v1.x/stream.html#c.uv_write_cb
 */
static void on_write_callback(uv_write_t *req, SN_UNUSED int status) {
    sock_loop_data_free((sock_loop_data *) req->data);
}

static void write_response(sock_loop_data *loop_data) {
    uv_buf_t buf = uv_buf_init(loop_data->http_data->response_buf, strlen(loop_data->http_data->response_buf) + 1);
    uv_write_t *write_req = malloc(sizeof(uv_write_t));
    if (write_req == NULL) {
        sn_log_err("Cannot create write_req object.\n");
        sock_loop_data_free(loop_data);
        return;
    }
    loop_data->write_req = write_req;
    loop_data->write_req->data = loop_data;
    uv_write(loop_data->write_req, (uv_stream_t *) loop_data->tcp_client, &buf, 1, on_write_callback);
}

/*
 * UV: uv_work_cb
 * https://docs.libuv.org/en/v1.x/threadpool.html#c.uv_work_cb
 */
static void on_work_callback(uv_work_t *work_req) {
    sock_loop_data *loop_data = work_req->data;
    loop_data->http_data->response_buf = strdup(
            "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<html><head><title>Hello World</title></head><h1>Hello World</h1></html>");
}

/*
 * UV: uv_after_work_cb
 * https://docs.libuv.org/en/v1.x/threadpool.html#c.uv_after_work_cb
 */
static void on_after_work_callback(uv_work_t *req, int status) {
    sock_loop_data *loop_data = req->data;
    if (status == UV_ECANCELED) {
        sn_log_info("Request was cancelled: %s\n", uv_strerror(status));
        sock_loop_data_free(loop_data);
        return;
    }
    write_response(loop_data);
}

// TODO: Implement a optimized header-length checker.
static sn_http_code_t check_fail_fast_err(sock_http_data *http_data, int minor_version) {
    size_t total_header_size = 0;
    if (minor_version != 0 && minor_version != 1) {
        return HTTP_VERSION_NOT_SUPPORTED;
    }
    if (http_data->path_length > HTTP_MAX_URI_SIZE) {
        return HTTP_URI_TOO_LONG;
    }
    for (int i = 0; i < http_data->header_length; i++) {
        size_t header_length = http_data->headers[i].name_len + http_data->headers[i].value_len;
        if (header_length > HTTP_MAX_SINGLE_HEADER_SIZE) {
            return HTTP_REQUEST_HEADER_FIELDS_TOO_LARGE;
        }
        total_header_size += header_length;
        if (total_header_size > HTTP_MAX_TOTAL_HEADER_SIZE) {
            return HTTP_REQUEST_HEADER_FIELDS_TOO_LARGE;
        }
    }
    return -1;
}

static void send_fail_fast_response(sock_loop_data *loop_data, sn_http_code_t code) {
    const char *response_text = sn_http_get_description(code);
    sn_log_err("Http early return with Code=%d, Details=%s\n", code, response_text);
    asprintf(&(loop_data->http_data->response_buf),
             "HTTP/1.1 %d %s\r\nContent-Type: text/plain\r\nContent-Length: %lu\r\n\r\n%s",
             code, response_text, strlen(response_text) + 1, response_text
    );
    write_response(loop_data);
}

static void dispatch_request(sock_loop_data *loop_data) {
    loop_data->work_req = malloc(sizeof(uv_work_t));
    if (loop_data->work_req == NULL) {
        sn_log_err("Cannot create work_req object.\n");
        clear_loop_request((uv_stream_t *) loop_data->tcp_client, NULL);
        return;
    }
    loop_data->work_req->data = loop_data;
    uv_queue_work(uv_default_loop(), loop_data->work_req, on_work_callback, on_after_work_callback);
}

/*
 * UV: uv_alloc_cb
 * https://docs.libuv.org/en/v1.x/handle.html#c.uv_alloc_cb
 */
static void on_buffer_alloc_callback(SN_UNUSED uv_handle_t *handle, SN_UNUSED size_t size, uv_buf_t *buf) {
    char *base = (char *) calloc(1, INC_SOCKET_BUF_SIZE);

    if (base == NULL)
        *buf = uv_buf_init(NULL, 0);
    else {
        *buf = uv_buf_init(base, INC_SOCKET_BUF_SIZE);
    }
}

/*
 * UV: uv_read_cb
 * https://docs.libuv.org/en/v1.x/stream.html#c.uv_read_cb
 */
static void on_read_callback(uv_stream_t *stream, ssize_t buf_size, const uv_buf_t *buf) {
    sn_http_code_t fail_fast_code;
    sock_loop_data *loop_data = stream->data;

    if (buf_size == -1 || buf_size == UV_EOF) {
        sn_log_err("Cannot read socket: %zd\n", buf_size);
        clear_loop_request(stream, buf);
        return;
    }

    if (loop_data->http_data == NULL) {
        loop_data->http_data = malloc(sizeof(sock_http_data));
        if (loop_data->http_data == NULL) {
            sn_log_err("Cannot create http_data object.\n");
            clear_loop_request(stream, buf);
            return;
        }
        loop_data->http_data->header_length = HTTP_MAX_HEADER_COUNT;
    }

    sock_http_data *http_data = loop_data->http_data;
    http_data->buf_cursor = http_data->buf_length;
    http_data->buf_length = http_data->buf_length + buf_size;
    if (http_data->buf_length == 0) {
        memcpy(http_data->request_buf, buf->base, buf_size);
    } else {
        void *allocated_memory = realloc(http_data->request_buf, http_data->buf_length);
        if (allocated_memory == NULL) {
            sn_log_err("Cannot allocate memory for incoming stream.\n");
            clear_loop_request(stream, buf);
            return;
        }
        http_data->request_buf = allocated_memory;
        memcpy(http_data->request_buf + http_data->buf_cursor, buf->base, buf_size);
    }

    int minor_version;
    int req_size = phr_parse_request(http_data->request_buf, http_data->buf_length, &(http_data->method),
                                     &http_data->method_length, &(http_data->path), &http_data->path_length,
                                     &minor_version, http_data->headers, &http_data->header_length,
                                     http_data->buf_cursor);

    if (req_size == -1) {
        sn_log_err("Cannot parse the HTTP request.\n");
        clear_loop_request(stream, buf);
        return;
    }

    free(buf->base);

    if (req_size == -2) {
        return;
    }

    uv_read_stop(stream);
    fail_fast_code = check_fail_fast_err(http_data, minor_version);

    if (fail_fast_code == -1) {
        dispatch_request(loop_data);
        return;
    }

    send_fail_fast_response(loop_data, fail_fast_code);
}

/*
 * UV: uv_connection_cb
 * https://docs.libuv.org/en/v1.x/stream.html#c.uv_connection_cb
 */
static void on_connection_callback(uv_stream_t *server, int status) {
    int accept_response;

    if (status == -1) {
        sn_log_err("Cannot accept connection: %d\n", status);
        return;
    }

    sock_loop_data *loop_data = malloc(sizeof(sock_loop_data));
    if (loop_data == NULL) {
        sn_log_err("Cannot create s_sock_loop_data object.\n");
        return;
    }

    uv_tcp_t *tcp_client = malloc(sizeof(uv_tcp_t));
    if (tcp_client == NULL) {
        sock_loop_data_free(loop_data);
        sn_log_err("Cannot create tcp_client object.\n");
        return;
    }

    tcp_client->data = loop_data;
    loop_data->tcp_client = tcp_client;
    uv_tcp_init(uv_default_loop(), tcp_client);

    accept_response = uv_accept(server, (uv_stream_t *) tcp_client);

    if (accept_response == 0) {
        uv_read_start((uv_stream_t *) tcp_client, on_buffer_alloc_callback, on_read_callback);
    } else {
        sn_log_err("Cannot accept connection: %d\n", accept_response);
        sock_loop_data_free(loop_data);
    }
}

int sn_listen(int port) {
    uv_tcp_t server;
    struct sockaddr_in addr;

    uv_ip4_addr("0.0.0.0", port, &addr);
    uv_tcp_init(uv_default_loop(), &server);
    uv_tcp_bind(&server, (struct sockaddr *) &addr, 0);

    int r = uv_listen((uv_stream_t *) &server, TCP_MAX_CON_BACKLOG, on_connection_callback);
    if (r) {
        sn_log_fatal("Cannot listen port. %s\n", uv_strerror(r));
        exit(EXIT_FAILURE);
    }
    sn_log_debug("Server is ready and accepts connection on %d\n", port);
    return uv_run(uv_default_loop(), UV_RUN_DEFAULT);
}