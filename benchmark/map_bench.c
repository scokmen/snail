#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include "snail.h"

typedef struct {
    int val;
} map_data;

const int NUMBER_OF_ITERATION   = 3;
const int NUMBER_OF_RAND_ACCESS = 1000;
const int NUMBER_OF_KEYS        = 4096;

static map_data *new_map_data() {
    map_data * data = malloc(sizeof (map_data));
    data->val = 1;
    return data;
}

static void map_destructor(const char* key, void *data) {
    if (key == NULL) {
        exit(EXIT_FAILURE);
    }
    free(data);
}

static const char *generate_key(int length) {
    char* str = calloc(length + 1, (sizeof(char)));
    if (str == NULL) {
        return NULL;
    }
    for (int i = 0; i < length; i++){
        str[i] = (char) (rand() % 26 + 65);
    }
    str[length] = 0;
    return str;
}

static const char *pick_key(const char **keys) {
    return keys[rand() % NUMBER_OF_KEYS];
}

static void run_map_bench(int16_t bucket_size, int key_length) {
    sn_map_t map;
    const char** keys = calloc(NUMBER_OF_KEYS, sizeof (char*));
    if (keys == NULL) {
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < NUMBER_OF_KEYS; i++) {
        keys[i] = generate_key(key_length);
        if (keys[i] == NULL) {
            exit(EXIT_FAILURE);
        }
    }

    if (sn_map_init(&map, bucket_size, map_destructor) != 0) {
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < 4096; i++) {
        if (sn_map_set(&map, keys[i], new_map_data())) {
            exit(EXIT_FAILURE);
        }
    }

    for (int iteration = 0; iteration < NUMBER_OF_ITERATION; iteration++) {
        // Random Access
        for (int i = 0; i < NUMBER_OF_RAND_ACCESS; i++) {
            const char* key = pick_key(keys);
            if (sn_map_has(&map, key)) {
                void* data = sn_map_get(&map, key);
                if ((*(int*)data) != 1) {
                    exit(EXIT_FAILURE);
                }
            }
        }

        // Random Overwrite
        for (int i = 0; i < NUMBER_OF_RAND_ACCESS; i++) {
            const char* key = pick_key(keys);
            if (sn_map_set(&map, key, new_map_data())) {
                exit(EXIT_FAILURE);
            }
        }

        // Random Delete
        for (int i = 0; i < NUMBER_OF_RAND_ACCESS; i++) {
            const char* key = pick_key(keys);
            sn_map_del(&map, key);
        }
    }

    for (int i = 0; i < 4096; i++) {
        free((char*)keys[i]);
    }

    sn_map_destroy(&map);
    free(keys);
}

int main() {
    srand(time(NULL));

    run_map_bench(1024, 8);
    run_map_bench(1024, 2);
    run_map_bench(1024, 1);
    run_map_bench(128, 8);
    run_map_bench(128, 2);
    run_map_bench(128, 1);
    run_map_bench(16, 8);
    run_map_bench(16, 2);
    run_map_bench(16, 1);
    run_map_bench(4, 8);
    run_map_bench(4, 2);
    run_map_bench(4, 1);

    exit(EXIT_SUCCESS);
}