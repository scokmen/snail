#include <stdlib.h>
#include <string.h>
#include "snail.h"
#include "../sn_common.h"

struct sn_map_node_s {
    void *data;
    const char *key;
    sn_map_t *map;
};

static int compare_keys(void *given, void *data) {
    const char *key = given;
    const char *target = ((struct sn_map_node_s *) data)->key;
    return key[0] == target[0] ? strcmp(key+1, target+1) : -1;
}

static void destroy_key_value(void *data) {
    struct sn_map_node_s *node = data;
    node->map->destructor(node->key, node->data);
    free(node);
    data = NULL;
}

static struct sn_map_node_s *create_node(sn_map_t *map, const char *key, void *data) {
    struct sn_map_node_s *node;
    MALLOC_OR_RETURN_NULL(node, struct sn_map_node_s, 1)
    node->key = key;
    node->data = data;
    node->map = map;
    return node;
}

static struct sn_map_node_s *search_node(sn_map_t *map, const char *key, int *target_bucket) {
    *target_bucket = (int) (djb2_hash((unsigned char *) key) % map->bucket_size);
    sn_dlist_t linked_list = map->buckets[*target_bucket];
    if (linked_list.size == 0) {
        return NULL;
    }
    return sn_dlist_get(&linked_list, (void *) key, compare_keys);
}

int sn_map_init(sn_map_t *map, uint16_t bucket_size, sn_map_destructor destructor) {
    map->bucket_size = bucket_size;
    map->destructor = destructor;
    map->buckets = calloc(bucket_size, sizeof(sn_dlist_t));

    if (map->buckets == NULL) {
        return SN_ENOMEM;
    }
    for (int i = 0; i < bucket_size; i++) {
        sn_dlist_init(&map->buckets[i], destroy_key_value);
    }
    return 0;
}

size_t sn_map_len(sn_map_t *map) {
    size_t length = 0;
    for (int i = 0; i < map->bucket_size; i++) {
        length += map->buckets[i].size;
    }
    return length;
}

int sn_map_set(sn_map_t *map, const char *key, void *data) {
    int bucket;
    struct sn_map_node_s *node = search_node(map, key, &bucket);
    if (node != NULL) {
        if (map->destructor != NULL) {
            map->destructor(node->key, node->data);
        }
        node->key = key;
        node->data = data;
        return 0;
    }
    node = create_node(map, key, data);
    if (node == NULL) {
        return SN_ENOMEM;
    }
    return sn_dlist_push(&(map->buckets[bucket]), node);
}

void *sn_map_get(sn_map_t *map, const char *key) {
    int bucket;
    struct sn_map_node_s *node = search_node(map, key, &bucket);
    return (node == NULL) ? NULL : node->data;
}

bool sn_map_has(sn_map_t *map, const char *key) {
    return (sn_map_get(map, key) != NULL);
}

void sn_map_del(sn_map_t *map, const char *key) {
    int bucket = (int) (djb2_hash((unsigned char *) key) % map->bucket_size);
    sn_dlist_del(&(map->buckets[bucket]), (void *) key, compare_keys);
}

void sn_map_destroy(sn_map_t *map) {
    for (int i = 0; i < map->bucket_size; i++) {
        sn_dlist_destroy(&(map->buckets[i]));
    }
    free(map->buckets);
    map->buckets = NULL;
}