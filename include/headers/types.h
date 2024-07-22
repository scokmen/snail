#ifndef SNAIL_TYPES_H
#define SNAIL_TYPES_H

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

typedef void (*sn_data_destructor)(void *data);
typedef int  (*sn_comparator)     (void *given, void *data);
typedef void (*sn_map_destructor) (const char *key, void *data);

typedef struct sn_dlist_node_t {
    void *data;
    struct sn_dlist_node_t *next;
    struct sn_dlist_node_t *prev;
} sn_dlist_node_t;

typedef struct {
    size_t size;
    sn_dlist_node_t *head;
    sn_dlist_node_t *tail;
    sn_data_destructor destructor;
} sn_dlist_t;

SN_NONNULL(1)
void sn_dlist_init(sn_dlist_t *list, sn_data_destructor destructor);

SN_NONNULL(1)
size_t sn_dlist_len(sn_dlist_t *list);

SN_NONNULL(1, 2)
int sn_dlist_push(sn_dlist_t *list, void *data);

SN_NONNULL(1)
void* sn_dlist_pop(sn_dlist_t *list) SN_USE_RESULT;

SN_NONNULL(1)
void* sn_dlist_shift(sn_dlist_t *list) SN_USE_RESULT;

SN_NONNULL(1, 2)
int sn_dlist_unshift(sn_dlist_t *list, void *data);

SN_NONNULL(1)
void *sn_dlist_first(sn_dlist_t *list);

SN_NONNULL(1)
void *sn_dlist_last(sn_dlist_t *list);

SN_NONNULL(1, 2, 3)
void *sn_dlist_get(sn_dlist_t *list, void *given, sn_comparator comparator);

SN_NONNULL(1, 2, 3)
bool sn_dlist_has(sn_dlist_t *list, void *given, sn_comparator comparator);

SN_NONNULL(1, 2, 3)
bool sn_dlist_del(sn_dlist_t *list, void *given, sn_comparator comparator);

SN_NONNULL(1, 2)
void sn_dlist_collect(sn_dlist_t *list, void **items, size_t limit);

SN_NONNULL(1)
void sn_dlist_destroy(sn_dlist_t *list);

typedef struct {
    uint16_t bucket_size;
    sn_dlist_t *buckets;
    sn_map_destructor destructor;
} sn_map_t;

SN_NONNULL(1)
int sn_map_init(sn_map_t *map, uint16_t bucket_size, sn_map_destructor destructor);

SN_NONNULL(1)
size_t sn_map_len(sn_map_t *map);

SN_NONNULL(1, 2, 3)
int sn_map_set(sn_map_t *map, const char *key, void *data);

SN_NONNULL(1, 2)
void *sn_map_get(sn_map_t *map, const char *key);

SN_NONNULL(1, 2)
bool sn_map_has(sn_map_t *map, const char *key);

SN_NONNULL(1, 2)
void sn_map_del(sn_map_t *map, const char *key);

SN_NONNULL(1)
void sn_map_destroy(sn_map_t *map);

#endif //SNAIL_TYPES_H
