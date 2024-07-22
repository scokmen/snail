#include <stdlib.h>
#include <string.h>
#include "../sn_common.h"
#include "snail.h"

#define MIDDLEWARE_CAP 4
#define MIDDLEWARE_EXT 4

int sn_routing_init(sn_router_t **router, sn_http_method_t method, const char *path, sn_route_handler_cb action) {
    sn_router_t *new_router;
    MALLOC_OR_RETURN_ERR(new_router, sn_router_t, 1)
    new_router->middlewares.handlers = malloc(sizeof (sn_route_handler_cb) * MIDDLEWARE_CAP);
    if (new_router->middlewares.handlers == NULL) {
        free(router);
        return SN_ENOMEM;
    }
    new_router->middlewares.size = 0;
    new_router->middlewares.capacity = MIDDLEWARE_CAP;
    new_router->path = path;
    new_router->method = method;
    new_router->action = action;
    new_router->type = SN_ROUTER;
    *router = new_router;
    return 0;
}

int sn_routing_init_group(sn_route_group_t **route_group, const char *path) {
    int error;
    sn_map_t *routes;
    sn_route_group_t *new_route_group;
    MALLOC_OR_RETURN_ERR(new_route_group, sn_route_group_t, 1)
    new_route_group->middlewares.handlers = malloc(sizeof (sn_route_handler_cb) * MIDDLEWARE_CAP);
    if (new_route_group->middlewares.handlers == NULL) {
        free(new_route_group);
        return SN_ENOMEM;
    }
    error = sn_map_init(&routes, 8, NULL);
    if (error != 0) {
        free(new_route_group->middlewares.handlers);
        free(new_route_group);
        return error;
    }
    new_route_group->path = path;
    new_route_group->middlewares.size = 0;
    new_route_group->middlewares.capacity = MIDDLEWARE_CAP;
    new_route_group->type = SN_ROUTER_GROUP;
    *route_group = new_route_group;
    return 0;
}

int sn_routing_add_group(sn_route_group_t *route_group, sn_handler_t *handler) {
    return sn_map_set(route_group->routes, handler->path, handler);
}

int sn_routing_add_middleware(sn_handler_t *handler, sn_route_handler_cb middleware) {
    if (handler->middlewares.size < handler->middlewares.capacity) {
        handler->middlewares.handlers[handler->middlewares.size++] = middleware;
        return 0;
    }
    void *ext = realloc(handler->middlewares.handlers, (sizeof (sn_route_handler_cb) * (handler->middlewares.capacity + MIDDLEWARE_EXT)));
    if (ext == NULL) {
        return SN_ENOMEM;
    }
    handler->middlewares.capacity += MIDDLEWARE_EXT;
    handler->middlewares.handlers = ext;
    handler->middlewares.handlers[handler->middlewares.size++] = middleware;
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
