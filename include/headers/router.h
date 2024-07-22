#ifndef SNAIL_ROUTER_H
#define SNAIL_ROUTER_H

#define MIDDLEWARE_LIMIT (4)

typedef enum {
    SN_UNKNOWN_HANDLER = 0,
    SN_ROUTER,
    SN_ROUTER_GROUP
} sn_handler_type;

#define HANDLER_PROPS const char *path;                                       \
                      sn_handler_type type;                                   \
                      struct {                                                \
                          int size;                                           \
                          int capacity;                                       \
                          sn_route_handler_cb *handlers;                      \
                      } middlewares;                                          \

typedef struct {
    sn_map_t *context;
} sn_route_ctx_t;

typedef struct {
    const char *path;
    sn_map_t *headers;
    sn_map_t *queries;
} sn_http_req_t;

typedef struct {
    sn_map_t *headers;
} sn_http_res_t;

typedef sn_http_res_t *(*sn_route_handler_cb)(sn_http_req_t *req, sn_route_ctx_t *ctx);

typedef struct {
    HANDLER_PROPS
} sn_handler_t;

typedef struct {
    HANDLER_PROPS
    sn_http_method_t method;
    sn_route_handler_cb action;
} sn_router_t;

typedef struct {
    HANDLER_PROPS
    sn_map_t *routes;
} sn_route_group_t;

SN_NONNULL(1, 3, 4)
int sn_routing_init(sn_router_t **router, sn_http_method_t method, const char *path, sn_route_handler_cb action);

SN_NONNULL(1, 2)
int sn_routing_init_group(sn_route_group_t **route_group, const char *path);

SN_NONNULL(1, 2)
int sn_routing_add_group(sn_route_group_t *route_group, sn_handler_t *handler);

SN_NONNULL(1, 2)
int sn_routing_add_middleware(sn_handler_t *handler, sn_route_handler_cb middleware);

SN_NONNULL(1)
void sn_routing_destroy(sn_handler_t *handler);

#endif //SNAIL_ROUTER_H
