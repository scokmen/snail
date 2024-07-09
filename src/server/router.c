#include <stdlib.h>
#include "../sn_common.h"
#include "snail.h"

int sn_routing_router_init(sn_router_t **router, sn_http_method_t method, const char *path, sn_route_handler_cb action) {
    MALLOC_OR_RETURN_ERR(*router, sn_router_t)
    (*router)->path = path;
    (*router)->middleware = 0;
    (*router)->method = method;
    (*router)->action = action;
    (*router)->type = SN_ROUTER;
    return 0;
}

int sn_routing_rgroup_init(sn_route_group_t **route_group, const char *path) {
    int error;
    sn_map_t *routes;
    MALLOC_OR_RETURN_ERR(*route_group, sn_route_group_t)
    error = sn_map_init(&routes, 8);
    if (error != 0) {
        return error;
    }
    (*route_group)->path = path;
    (*route_group)->middleware = 0;
    (*route_group)->type = SN_ROUTER_GROUP;
    return 0;
}

void sn_routing_rgroup_add_submodule(sn_route_group_t *route_group, sn_handler_t *handler) {
    sn_map_set(route_group->routes, handler->path, handler, (sn_map_unregister_cb) sn_routing_destroy);
}

int sn_routing_add_middleware(sn_handler_t *handler, sn_route_handler_cb middleware) {
    if (handler->middleware == MIDDLEWARE_LIMIT) {
        return SN_ERR_MIDDLEWARE_LIMIT_EXCEEDED;
    }
    handler->middlewares[handler->middleware++] = middleware;
    return 0;
}

void sn_routing_destroy(sn_handler_t *handler) {
    if (handler->type == SN_UNKNOWN_HANDLER) {
        return;
    }
    if (handler->type == SN_ROUTER) {
        free(handler);
        return;
    }
    if (handler->type == SN_ROUTER_GROUP) {
        sn_map_destroy(((sn_route_group_t*)handler)->routes);
        free(handler);
    }
}
