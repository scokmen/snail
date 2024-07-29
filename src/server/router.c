#include <stdlib.h>
#include "snail.h"

void sn_route_handler_init(sn_route_handler_t *route_handler, sn_http_method_t method, const char *path, sn_req_handler handler) {
    sn_dlist_init(&route_handler->middlewares, NULL);
    route_handler->path = path;
    route_handler->method = method;
    route_handler->action = handler;
    route_handler->type = SN_ROUTE_HANDLER;
}

void sn_route_group_init(sn_route_group_t *route_group, const char *path) {
    sn_dlist_init(&route_group->middlewares, NULL);
    sn_dlist_init(&route_group->routes, (sn_data_destructor) sn_route_destroy);
    route_group->path = path;
    route_group->type = SN_ROUTE_GROUP;
}

int sn_route_add_subgroup(sn_route_group_t *route_group, sn_handler_t *handler) {
    return sn_dlist_push(&route_group->routes, handler);
}

int sn_route_add_middleware(sn_handler_t *handler, sn_req_handler middleware) {
    return sn_dlist_push(&handler->middlewares, middleware);
}

void sn_route_destroy(sn_handler_t *handler) {
    if (handler->type == SN_UNKNOWN_HANDLER) {
        return;
    }
    sn_dlist_destroy(&handler->middlewares);
    if (handler->type == SN_ROUTE_GROUP) {
        sn_dlist_destroy(&((sn_route_group_t*)handler)->routes);
        free(handler);
    }
}
