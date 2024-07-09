#ifndef SNAIL_MAP_H
#define SNAIL_MAP_H

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

typedef void (*sn_map_unregister_cb)(const char* key, void *data);

typedef struct sn_map_lln_t {
    bool clean;
    void *data;
    const char *label;
    struct sn_map_lln_t *next;
    sn_map_unregister_cb unregister_cb;
} sn_map_lln_t;

typedef struct {
    int count;
    sn_map_lln_t *head;
} sn_map_ll_t;

typedef struct {
    uint16_t bucket_size;
    sn_map_ll_t *buckets;
} sn_map_t;

SN_NONNULL(1)
int sn_map_init(sn_map_t **map, uint16_t bucket_size);

SN_NONNULL(1)
void sn_map_destroy(sn_map_t *map);

SN_NONNULL(1, 2, 3, 4)
int sn_map_set(sn_map_t *map, const char *key, void *data, sn_map_unregister_cb unregister_cb);

SN_NONNULL(1, 2)
void *sn_map_get(sn_map_t *map, const char *key);

SN_NONNULL(1, 2)
void sn_map_del(sn_map_t *map, const char *key);

SN_NONNULL(1, 2)
bool sn_map_has(sn_map_t *map, const char *key);

SN_NONNULL(1)
size_t sn_map_length(sn_map_t *map);

#endif //SNAIL_MAP_H
