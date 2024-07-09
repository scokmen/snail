#include <stdlib.h>
#include <string.h>
#include "../sn_common.h"
#include "snail.h"

static sn_map_lln_t *create_linked_list_node(const char *key, void *data, sn_map_unregister_cb unregister_cb) {
    sn_map_lln_t *node;
    MALLOC_OR_RETURN_NULL(node, sn_map_lln_t)
    node->clean = true;
    node->next = NULL;
    node->data = data;
    node->label = key;
    node->unregister_cb = unregister_cb;
    return node;
}

static sn_map_lln_t *find_label_node(sn_map_t *map, const char *key, int* target_bucket) {
    *target_bucket = (int) (djb2_hash((unsigned char *) key) % map->bucket_size);
    sn_map_ll_t linked_list = map->buckets[*target_bucket];
    sn_map_lln_t *node = NULL;

    if (linked_list.count == 0) {
        return NULL;
    }

    node = linked_list.head;
    while (node != NULL) {
        if (!node->clean) {
            node = node->next;
            continue;
        }
        if (node->label[0] == key[0] && strcmp(node->label, key) == 0) {
            return node;
        }
        node = node->next;
    }

    return NULL;
}

int sn_map_init(sn_map_t **map, uint16_t bucket_size) {
    MALLOC_OR_RETURN_ERR(*map, sn_map_t)
    (*map)->bucket_size = bucket_size;
    (*map)->buckets = calloc(bucket_size, sizeof(sn_map_ll_t));
    if ((*map)->buckets == NULL) {
        free(map);
        return SN_ERR_MALLOC_FAILED;
    }
    return 0;
}

void sn_map_destroy(sn_map_t *map) {
    sn_map_lln_t *node, *next;
    for (int i = 0; i < map->bucket_size; i++) {
        node = map->buckets[i].head;
        if (node == NULL) {
            continue;
        }
        while (node != NULL) {
            next = node->next;
            if (node->clean && node->unregister_cb != NULL) {
                node->unregister_cb(node->label, node->data);
            }
            free(node);
            node = next;
        }
    }
    free(map->buckets);
    free(map);
}

int sn_map_set(sn_map_t *map, const char *key, void *data, sn_map_unregister_cb unregister_cb) {
    int bucket;
    sn_map_lln_t *node = find_label_node(map, key, &bucket);
    if (node != NULL) {
        if (node->clean && node->unregister_cb != NULL) {
            node->unregister_cb(node->label, node->data);
        }
        node->clean = true;
        node->unregister_cb = unregister_cb;
        node->data = data;
        return 0;
    }
    node = map->buckets[bucket].head;
    if (node == NULL) {
        node = create_linked_list_node(key, data, unregister_cb);
        if (node == NULL) {
            return SN_ERR_MALLOC_FAILED;
        }
        map->buckets[bucket].head = node;
        map->buckets[bucket].count++;
        return 0;
    }

    while (node->next != NULL) {
        if (!node->clean) {
            break;
        }
        node = node->next;
    }

    if (!node->clean) {
        node->clean = true;
        node->data = data;
        node->label = key;
        node->unregister_cb = unregister_cb;
        map->buckets[bucket].count++;
        return 0;
    }

    node->next = create_linked_list_node(key, data, unregister_cb);
    if (node->next != NULL) {
        map->buckets[bucket].count++;
        return 0;
    }
    return SN_ERR_MALLOC_FAILED;
}

void *sn_map_get(sn_map_t *map, const char *key) {
    int bucket;
    sn_map_lln_t *node = find_label_node(map, key, &bucket);
    return (node == NULL || !node->clean) ? NULL : node->data;
}

void sn_map_del(sn_map_t *map, const char *key) {
    int bucket;
    sn_map_lln_t *node = find_label_node(map, key, &bucket);
    if (node == NULL) {
        return;
    }
    if (node->unregister_cb != NULL) {
        node->unregister_cb(node->label, node->data);
    }
    node->clean = false;
    node->data = NULL;
    node->unregister_cb = NULL;
    map->buckets[bucket].count--;
}

bool sn_map_has(sn_map_t *map, const char *key) {
    int bucket;
    sn_map_lln_t *node = find_label_node(map, key, &bucket);
    return !(node == NULL || !node->clean);
}

size_t sn_map_length(sn_map_t *map) {
    size_t length = 0;
    for (int i = 0; i < map->bucket_size; i++) {
        length += map->buckets[i].count;
    }
    return length;
}
