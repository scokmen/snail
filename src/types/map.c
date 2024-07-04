#include <stdlib.h>
#include <printf.h>
#include <string.h>
#include <assert.h>
#include "snail.h"

SN_CONST_FN
static unsigned long djb2_hash(unsigned char *str) {
    int c;
    unsigned long hash = 5381;

    while ((c = *str++)) {
        hash = ((hash << 5) + hash) + c;
    }

    return hash;
}

static sn_map_lln_t *create_linked_list_node(const char *key, void *data, sn_map_unregister_cb unregister_cb) {
    sn_map_lln_t *node = malloc(sizeof (sn_map_lln_t));
    if (node == NULL) {
        return NULL;
    }
    node->active = true;
    node->next = NULL;
    node->data = data;
    node->label = key;
    node->unregister_cb = unregister_cb;
    return node;
}

static sn_map_lln_t *find_label_node(sn_map_t *map, const char *key, int* target_bucket, bool scan_inactive_nodes) {
    *target_bucket = (int) (djb2_hash((unsigned char *) key) % map->bucket_size);
    sn_map_ll_t linked_list = map->buckets[*target_bucket];
    sn_map_lln_t *node = NULL;

    if (!scan_inactive_nodes && linked_list.count == 0) {
        return NULL;
    }

    node = linked_list.head;
    while (node != NULL) {
        if (!scan_inactive_nodes && !node->active) {
            node = node->next;
            continue;
        }
        if (strcmp(node->label, key) == 0) {
            return node;
        }
        node = node->next;
    }

    return NULL;
}

sn_map_t *sn_map_init(uint16_t bucket_size) {
    assert(bucket_size > 0);
    sn_map_t *map = malloc(sizeof(sn_map_t));
    if (map == NULL) {
        return NULL;
    }
    map->bucket_size = bucket_size;
    map->buckets = calloc(bucket_size, sizeof(sn_map_ll_t));
    if (map->buckets == NULL) {
        free(map);
        return NULL;
    }
    return map;
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
            if (node->active && node->unregister_cb != NULL) {
                node->unregister_cb(node->data);
            }
            free(node);
            node = next;
        }
    }
    free(map->buckets);
    free(map);
}

bool sn_map_set(sn_map_t *map, const char *key, void *data, sn_map_unregister_cb unregister_cb) {
    int bucket;
    sn_map_lln_t *node = find_label_node(map, key, &bucket, true);
    if (node != NULL) {
        if (node->active && node->unregister_cb != NULL) {
            node->unregister_cb(node->data);
        }
        node->active = true;
        node->unregister_cb = unregister_cb;
        node->data = data;
        return true;
    }
    node = map->buckets[bucket].head;
    if (node == NULL) {
        node = create_linked_list_node(key, data, unregister_cb);
        if (node == NULL) {
            return false;
        }
        map->buckets[bucket].head = node;
        map->buckets[bucket].count++;
        return true;
    }

    while (node->next != NULL) {
        if (!node->active) {
            break;
        }
        node = node->next;
    }

    if (!node->active) {
        node->active = true;
        node->data = data;
        node->label = key;
        node->unregister_cb = unregister_cb;
        map->buckets[bucket].count++;
        return true;
    }

    node->next = create_linked_list_node(key, data, unregister_cb);
    if (node->next != NULL) {
        map->buckets[bucket].count++;
        return true;
    }
    return false;
}

void *sn_map_get(sn_map_t *map, const char *key) {
    int bucket;
    sn_map_lln_t *node = find_label_node(map, key, &bucket,  false);
    return (node == NULL || !node->active) ? NULL : node->data;
}

void sn_map_del(sn_map_t *map, const char *key) {
    int bucket;
    sn_map_lln_t *node = find_label_node(map, key, &bucket,  false);
    if (node == NULL) {
        return;
    }
    if (node->unregister_cb != NULL) {
        node->unregister_cb(node->data);
    }
    node->active = false;
    node->data = NULL;
    node->unregister_cb = NULL;
    map->buckets[bucket].count--;
}

bool sn_map_has(sn_map_t *map, const char *key) {
    int bucket;
    sn_map_lln_t *node = find_label_node(map, key, &bucket,  false);
    return !(node == NULL || !node->active);
}