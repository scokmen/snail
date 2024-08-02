#include "snail.h"
#include "sn_common.h"

static void destroy_node(sn_dlist_t *list, struct sn_dlist_node_s *node) {
    if (list->destructor != NULL) {
        list->destructor(node->data);
    }
    free(node);
    node = NULL;
}

static struct sn_dlist_node_s *search_node(sn_dlist_t *list, void *given, sn_comparator comparator) {
    struct sn_dlist_node_s *node;
    if (list->size > 0) {
        node = list->head;
        while (node != NULL) {
            if (comparator(given, node->data) == 0) {
                return node;
            }
            node = node->next;
        }
    }
    return NULL;
}

void sn_dlist_init(sn_dlist_t *list, sn_data_destructor destructor) {
    list->size = 0;
    list->head = NULL;
    list->tail = NULL;
    list->destructor = destructor;
}

size_t sn_dlist_len(sn_dlist_t *list) {
    return list->size;
}

int sn_dlist_push(sn_dlist_t *list, void *data) {
    struct sn_dlist_node_s *node;
    MALLOC_OR_RETURN_ERR(node, struct sn_dlist_node_s, 1)
    node->data = data;
    node->next = NULL;
    if (list->size == 0) {
        node->prev = NULL;
        list->head = node;
        list->tail = node;
    } else {
        node->prev = list->tail;
        list->tail->next = node;
        list->tail = node;
    }
    list->size++;
    return 0;
}

void *sn_dlist_pop(sn_dlist_t *list) {
    void *data;
    struct sn_dlist_node_s *node;
    if (list->size == 0) {
        return NULL;
    }
    if (list->size == 1) {
        node = list->tail;
        list->head = NULL;
        list->tail = NULL;
    } else {
        node = list->tail;
        list->tail = node->prev;
        list->tail->next = NULL;
    }
    data = node->data;
    free(node);
    node = NULL;
    list->size--;
    return data;
}

void* sn_dlist_shift(sn_dlist_t *list) {
    void *data;
    struct sn_dlist_node_s *node;
    if (list->size < 2) {
        return sn_dlist_pop(list);
    }
    node = list->head;
    data = node->data;
    list->head = node->next;
    list->head->prev = NULL;
    free(node);
    node = NULL;
    list->size--;
    return data;
}

int sn_dlist_unshift(sn_dlist_t *list, void *data) {
    struct sn_dlist_node_s *node;
    if (list->size == 0) {
        return sn_dlist_push(list, data);
    }
    MALLOC_OR_RETURN_ERR(node, struct sn_dlist_node_s, 1)
    node->data = data;
    node->prev = NULL;
    list->head->prev = node;
    node->next = list->head;
    list->head = node;
    list->size++;
    return 0;
}

void *sn_dlist_first(sn_dlist_t *list) {
    if (list->size == 0) {
        return NULL;
    }
    return list->head->data;
}

void *sn_dlist_last(sn_dlist_t *list) {
    if (list->size == 0) {
        return NULL;
    }
    return list->tail->data;
}

void *sn_dlist_get(sn_dlist_t *list, void *given, sn_comparator comparator) {
    struct sn_dlist_node_s *node = search_node(list, given, comparator);
    return (node != NULL) ? node->data : NULL;
}

bool sn_dlist_has(sn_dlist_t *list, void *given, sn_comparator comparator) {
    struct sn_dlist_node_s *node = NULL;
    if (list->size > 0) {
        node = sn_dlist_get(list, given, comparator);
    }
    return node != NULL;
}

bool sn_dlist_del(sn_dlist_t *list, void *given, sn_comparator comparator) {
    struct sn_dlist_node_s *node = search_node(list, given, comparator);
    if (node == NULL) {
        return false;
    }

    if (list->size == 1) {
        list->head = NULL;
        list->tail = NULL;
    } else if (node == list->head) {
        list->head = node->next;
        list->head->prev = NULL;
    } else if (node == list->tail) {
        list->tail = node->prev;
        list->tail->next = NULL;
    } else {
        node->prev->next = node->next;
        node->next->prev = node->prev;
    }

    destroy_node(list, node);
    list->size--;
    return true;
}

void sn_dlist_collect(sn_dlist_t *list, void **items, size_t limit) {
    int i = 0;
    struct sn_dlist_node_s *node = list->head;
    while (node != NULL && i < limit) {
        items[i++] = node->data;
        node = node->next;
    }
}

void sn_dlist_destroy(sn_dlist_t *list) {
    struct sn_dlist_node_s *next = NULL, *node = list->head;
    while (node != NULL) {
        next = node->next;
        destroy_node(list, node);
        node = next;
    }
}