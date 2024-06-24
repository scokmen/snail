#include <stdio.h>
#include <uv.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stddef.h>
#include <picohttpparser.h>
#include "../snail.h"
#include "s_sock_loop.h"

typedef struct s_sock_http_data {
    const char *path;
    const char *method;
    char *request_buf SN_BUFFER;
    char *response_buf SN_BUFFER;
    size_t buf_length;
    size_t buf_cursor;
    size_t path_length;
    size_t method_length;
    size_t header_length;
    struct phr_header headers[S_HTTP_MAX_HEADER_COUNT];
} s_sock_http_data;

typedef struct s_sock_loop_data {
    bool work_started;
    s_sock_http_data *http_data;
    uv_tcp_t *tcp_client;
    uv_work_t *work_req;
    uv_write_t *write_req;
} s_sock_loop_data;

/*
 * UV: uv_close_cb
 * https://docs.libuv.org/en/v1.x/handle.html#c.uv_close_cb
 */
static void close_tcp_client_callback(uv_handle_t *handle) {
    if (handle != NULL) {
        free(handle);
    }
}

static void s_sock_http_data_free(s_sock_http_data *data) {
    if (data == NULL) {
        return;
    }

    if (data->request_buf != NULL) {
        free(data->request_buf);
    }

    if (data->response_buf != NULL) {
        free(data->response_buf);
    }
}

static void s_sock_loop_data_free(s_sock_loop_data *data) {
    if (data == NULL) {
        return;
    }

    if (data->tcp_client != NULL) {
        uv_close((uv_handle_t *) data->tcp_client, close_tcp_client_callback);
    }

    s_sock_http_data_free(data->http_data);

    if (data->work_req != NULL) {
        free(data->work_req);
    }

    if (data->write_req != NULL) {
        free(data->write_req);
    }

    free(data);
}

static inline SN_INLINE void fail_fast(const char *message, s_sock_loop_data *data, const uv_buf_t *buffer, uv_stream_t *stream) {
    fprintf(stderr, "%s %s:%d\n", message, __FILE__, __LINE__);
    if (stream != NULL) {
        uv_read_stop(stream);
    }
    if (data != NULL) {
        s_sock_loop_data_free(data);
    }
    if (buffer != NULL) {
        free(buffer->base);
    }
}

/*
 * UV: uv_write_cb
 * https://docs.libuv.org/en/v1.x/stream.html#c.uv_write_cb
 */
static void handle_response_sent(uv_write_t *req, SN_UNUSED int status) {
    s_sock_loop_data_free((s_sock_loop_data *) req->data);
}

/*
 * UV: uv_work_cb
 * https://docs.libuv.org/en/v1.x/threadpool.html#c.uv_work_cb
 */
static void handle_request(uv_work_t *work_req) {
    s_sock_loop_data *loop_data = work_req->data;
    loop_data->http_data->response_buf = strdup(
            "HTTP/1.1 200 OK\nContent-Type: text/html\n\n<html><head><title>Hello World</title></head><h1>Hello World</h1></html>");
}

/*
 * UV: uv_after_work_cb
 * https://docs.libuv.org/en/v1.x/threadpool.html#c.uv_after_work_cb
 */
static void handle_response(uv_work_t *req, int status) {
    s_sock_loop_data *data = req->data;
    if (status == UV_ECANCELED) {
        fail_fast("Request was cancelled!", data, NULL, NULL);
        return;
    }
    uv_buf_t buf = uv_buf_init(data->http_data->response_buf, strlen(data->http_data->response_buf) + 1);
    data->write_req = malloc(sizeof(uv_write_t));
    if (data->write_req == NULL) {
        fail_fast("Cannot allocate memory!", data, NULL, NULL);
        return;
    }
    data->write_req->data = data;
    uv_write(data->write_req, (uv_stream_t *) data->tcp_client, &buf, 1, handle_response_sent);
}

/*
 * UV: uv_alloc_cb
 * https://docs.libuv.org/en/v1.x/handle.html#c.uv_alloc_cb
 */
static void allocate_buffer_callback(SN_UNUSED uv_handle_t *handle, SN_UNUSED size_t size, uv_buf_t *buf) {
    char *base = (char *) calloc(1, S_INC_SOCKET_BUF_SIZE);

    if (base == NULL)
        *buf = uv_buf_init(NULL, 0);
    else {
        *buf = uv_buf_init(base, S_INC_SOCKET_BUF_SIZE);
    }
}

/*
 * UV: uv_read_cb
 * https://docs.libuv.org/en/v1.x/stream.html#c.uv_read_cb
 */
static void socket_read_callback(uv_stream_t *stream, ssize_t buf_size, const uv_buf_t *buf) {
    s_sock_loop_data *data = stream->data;

    if (buf_size == -1 || buf_size == UV_EOF) {
        fail_fast("Cannot read socket!", data, buf, stream);
        return;
    }

    if (data->http_data == NULL) {
        data->http_data = malloc(sizeof(s_sock_http_data));
        if (data->http_data == NULL) {
            fail_fast("Cannot allocate memory!", data, buf, stream);
            return;
        }
        data->http_data->header_length = S_HTTP_MAX_HEADER_COUNT;
    }

    s_sock_http_data *http_data = data->http_data;
    http_data->buf_cursor = http_data->buf_length;
    http_data->buf_length = http_data->buf_length + buf_size;
    if (http_data->request_buf == NULL) {
        http_data->request_buf = buf->base;
    } else {
        void *allocated_memory = realloc(http_data->request_buf, http_data->buf_length);
        if (allocated_memory == NULL) {
            fail_fast("Cannot allocate memory!", data, buf, stream);
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
        fail_fast("Cannot parse the HTTP request!", data, buf, stream);
        return;
    }

    if (req_size == -2) {
        return;
    }

    data->work_req = malloc(sizeof(uv_work_t));
    if (data->work_req == NULL) {
        fail_fast("Cannot create request!", data, buf, stream);
        return;
    }
    data->work_req->data = data;
    data->work_started = true;
    uv_read_stop(stream);
    uv_queue_work(uv_default_loop(), data->work_req, handle_request, handle_response);
}

/*
 * UV: uv_connection_cb
 * https://docs.libuv.org/en/v1.x/stream.html#c.uv_connection_cb
 */
static void connection_callback(uv_stream_t *server, int status) {
    if (status == -1) {
        fail_fast("Cannot accept connection!", NULL, NULL, NULL);
        return;
    }

    s_sock_loop_data *event_data = malloc(sizeof(s_sock_loop_data));
    if (event_data == NULL) {
        fail_fast("Cannot allocate memory!", NULL, NULL, NULL);
        return;
    }

    uv_tcp_t *tcp_client = malloc(sizeof(uv_tcp_t));
    if (tcp_client == NULL) {
        fail_fast("Cannot allocate memory!", NULL, NULL, NULL);
        return;
    }

    tcp_client->data = event_data;
    event_data->tcp_client = tcp_client;
    uv_tcp_init(uv_default_loop(), tcp_client);

    if (uv_accept(server, (uv_stream_t *) tcp_client) == 0) {
        uv_read_start((uv_stream_t *) tcp_client, allocate_buffer_callback, socket_read_callback);
    } else {
        fail_fast("Cannot accept connection!", event_data, NULL, NULL);
    }
}

int s_listen(int port) {
    uv_tcp_t server;
    struct sockaddr_in addr;

    uv_ip4_addr("0.0.0.0", port, &addr);
    uv_tcp_init(uv_default_loop(), &server);
    uv_tcp_bind(&server, (struct sockaddr *) &addr, 0);

    int r = uv_listen((uv_stream_t *) &server, S_TCP_MAX_CON_BACKLOG, connection_callback);
    if (r) {
        return fprintf(stderr, "Cannot listen: %s.\n", uv_strerror(r));
    }
    return uv_run(uv_default_loop(), UV_RUN_DEFAULT);
}