#include <stdio.h>
#include <uv.h>
#include <stdlib.h>
#include <stdbool.h>

#include "s_core_buf.h"
#include "../snail.h"
#include "s_http_serv.h"
#include "s_sock_loop.h"

typedef struct s_sock_loop_data {
    s_buf req_buf;
    s_buf res_buf;
    bool work_started;
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

/*
 * SN: s_sock_loop_data
 * free loop data and close tcp client
 */
static void s_sock_loop_data_free(s_sock_loop_data *data) {
    if (data == NULL) {
        return;
    }

    if (data->req_buf.buffer != NULL) {
        free(data->req_buf.buffer);
    }

    if (data->res_buf.buffer != NULL) {
        free(data->res_buf.buffer);
    }

    if (data->tcp_client != NULL) {
        uv_close((uv_handle_t *) data->tcp_client, close_tcp_client_callback);
    }

    if (data->work_req != NULL) {
        free(data->work_req);
    }

    if (data->write_req != NULL) {
        free(data->write_req);
    }

    free(data);
}


/*
 * UV: uv_write_cb
 * https://docs.libuv.org/en/v1.x/stream.html#c.uv_write_cb
 */
static void handle_response_sent(uv_write_t *req, int status) {
    s_sock_loop_data_free((s_sock_loop_data *) req->data);
}

/*
 * UV: uv_work_cb
 * https://docs.libuv.org/en/v1.x/threadpool.html#c.uv_work_cb
 */
static void handle_request(uv_work_t *work_req) {
    s_sock_loop_data *loop_data = work_req->data;
    loop_data->res_buf = s_handle_request(loop_data->req_buf);
}

/*
 * UV: uv_after_work_cb
 * https://docs.libuv.org/en/v1.x/threadpool.html#c.uv_after_work_cb
 */
static void handle_response(uv_work_t *req, int status) {
    s_sock_loop_data *data = req->data;
    if (status == UV_ECANCELED) {
        s_sock_loop_data_free(data);
        return;
    }
    uv_buf_t buf = uv_buf_init(data->res_buf.buffer, data->res_buf.size + 1);
    data->write_req = malloc(sizeof(uv_write_t));
    if (data->write_req == NULL) {
        fprintf(stderr, "cannot allocate memory %s:%d", __FILE__, __LINE__);
        s_sock_loop_data_free(data);
        return;
    }
    data->write_req->data = data;
    uv_write(data->write_req, (uv_stream_t *) data->tcp_client, &buf, 1, handle_response_sent);
}

/*
 * UV: uv_alloc_cb
 * https://docs.libuv.org/en/v1.x/handle.html#c.uv_alloc_cb
 */
static void allocate_buffer_callback(uv_handle_t *handle, size_t size, uv_buf_t *buf) {
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
    struct s_sock_loop_data *data;
    data = stream->data;

    if (buf_size == -1 || buf_size == UV_EOF) {
        fprintf(stderr, "cannot read socket %s:%d", __FILE__, __LINE__);
        free(buf->base);
        s_sock_loop_data_free(data);
        return;
    }

    if (data->req_buf.size == 0) {
        data->req_buf = s_buf_init(buf->base, buf_size, S_INC_SOCKET_MAX_SIZE);
    } else if (buf_size > 0) {
        int extended_buf = s_buf_extend(&data->req_buf, buf->base, buf_size);
        if (extended_buf < 0) {
            fprintf(stderr, "cannot read socket %s:%d", __FILE__, __LINE__);
            free(buf->base);
            s_sock_loop_data_free(data);
            return;
        }
    }

    if (!data->work_started && buf_size < S_INC_SOCKET_BUF_SIZE) {
        data->work_req = malloc(sizeof(uv_work_t));
        if (data->work_req == NULL) {
            fprintf(stderr, "cannot create request %s:%d", __FILE__, __LINE__);
            free(buf->base);
            s_sock_loop_data_free(data);
            return;
        }
        data->work_req->data = data;
        data->work_started = true;
        uv_read_stop(stream);
        uv_queue_work(uv_default_loop(), data->work_req, handle_request, handle_response);
    }
}

/*
 * UV: uv_connection_cb
 * https://docs.libuv.org/en/v1.x/stream.html#c.uv_connection_cb
 */
static void connection_callback(uv_stream_t *serverInstance, int status) {
    if (status == -1) {
        fprintf(stderr, "cannot accept the connection %s:%d", __FILE__, __LINE__);
        return;
    }

    uv_tcp_t *tcp_client = malloc(sizeof(uv_tcp_t));
    if (tcp_client == NULL) {
        fprintf(stderr, "cannot allocate memory %s:%d", __FILE__, __LINE__);
        return;
    }

    s_sock_loop_data *event_data = malloc(sizeof(s_sock_loop_data));
    if (event_data == NULL) {
        fprintf(stderr, "cannot allocate memory %s:%d", __FILE__, __LINE__);
        return;
    }

    tcp_client->data = event_data;
    event_data->tcp_client = tcp_client;
    uv_tcp_init(uv_default_loop(), tcp_client);

    if (uv_accept(serverInstance, (uv_stream_t *) tcp_client) == 0) {
        uv_read_start((uv_stream_t *) tcp_client, allocate_buffer_callback, socket_read_callback);
    } else {
        fprintf(stderr, "cannot accept the connection %s:%d", __FILE__, __LINE__);
        s_sock_loop_data_free(event_data);
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
        return fprintf(stderr, "cannot listen: %s.\n", uv_strerror(r));
    }
    return uv_run(uv_default_loop(), UV_RUN_DEFAULT);
}