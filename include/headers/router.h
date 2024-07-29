#ifndef SNAIL_ROUTER_H
#define SNAIL_ROUTER_H

#define HANDLER_PROPS const char *path;                  \
                      sn_handler_type type;              \
                      sn_dlist_t middlewares;            \

typedef enum {
    SN_UNKNOWN_HANDLER = 0,
    SN_ROUTE_HANDLER,
    SN_ROUTE_GROUP
} sn_handler_type;

typedef struct {
    sn_map_t context;
} sn_route_ctx_t;

typedef struct {
    const char *path;
    sn_map_t headers;
    sn_map_t queries;
    sn_map_t params;
} sn_http_req_t;

typedef struct {
    sn_map_t headers;
} sn_http_res_t;

typedef sn_http_res_t *(*sn_req_handler)(sn_http_req_t *req, sn_route_ctx_t *ctx);

typedef struct {
    HANDLER_PROPS
} sn_handler_t;

typedef struct {
    HANDLER_PROPS
    sn_http_method_t method;
    sn_req_handler action;
} sn_route_handler_t;

typedef struct {
    HANDLER_PROPS
    sn_dlist_t routes;
} sn_route_group_t;

SN_NONNULL(1, 3, 4)
void sn_route_handler_init(sn_route_handler_t *route_handler, sn_http_method_t method, const char *path, sn_req_handler handler);

SN_NONNULL(1, 2)
void sn_route_group_init(sn_route_group_t *route_group, const char *path);

SN_NONNULL(1, 2)
int sn_route_add_subgroup(sn_route_group_t *route_group, sn_handler_t *handler);

SN_NONNULL(1, 2)
int sn_route_add_middleware(sn_handler_t *handler, sn_req_handler middleware);

SN_NONNULL(1)
void sn_route_destroy(sn_handler_t *handler);

#endif //SNAIL_ROUTER_H
