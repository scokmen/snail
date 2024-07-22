#include <snail.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>

typedef struct {
    int val;
} map_data;

static map_data *new_map_data() {
    map_data * data = malloc(sizeof (map_data));
    data->val = 1;
    return data;
}

static void key_value_destructor(const char* key, void *data) {
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

static const char* pick_key(const char** keys, int count) {
    return keys[rand() % count];
}

static void run_map_bench(int16_t bucket_size, int count, int key_length, int num_of_iteration, int num_of_random_access) {
    sn_map_t map;
    const char** keys = calloc(count, sizeof (char*));
    if (keys == NULL) {
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < count; i++) {
        keys[i] = generate_key(key_length);
        if (keys[i] == NULL) {
            exit(EXIT_FAILURE);
        }
    }

    if (sn_map_init(&map, bucket_size, key_value_destructor) != 0) {
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < count; i++) {
        if (sn_map_set(&map, keys[i], new_map_data())) {
            exit(EXIT_FAILURE);
        }
    }

    for (int iteration = 0; iteration < num_of_iteration; iteration++) {
        // Random Access
        for (int i = 0; i < num_of_random_access; i++) {
            const char* key = pick_key(keys, count);
            if (sn_map_has(&map, key)) {
                void* data = sn_map_get(&map, key);
                if ((*(int*)data) != 1) {
                    exit(EXIT_FAILURE);
                }
            }
        }

        // Random Overwrite
        for (int i = 0; i < num_of_random_access; i++) {
            const char* key = pick_key(keys, count);
            if (sn_map_set(&map, key, new_map_data())) {
                exit(EXIT_FAILURE);
            }
        }

        // Random Delete
        for (int i = 0; i < 10000; i++) {
            const char* key = pick_key(keys, count);
            sn_map_del(&map, key);
        }
    }

    for (int i = 0; i < count; i++) {
        free((char*)keys[i]);
    }

    sn_map_destroy(&map);
    free(keys);
}

int main() {
    srand(time(NULL));

    run_map_bench(1024, 4096, 8, 3, 1000);
    run_map_bench(1024, 4096, 2, 3, 1000);
    run_map_bench(1024, 4096, 1, 3, 1000);
    run_map_bench(128, 4096, 8, 3, 1000);
    run_map_bench(128, 4096, 2, 3, 1000);
    run_map_bench(128, 4096, 1, 3, 1000);
    run_map_bench(16, 4096, 8, 3, 1000);
    run_map_bench(16, 4096, 2, 3, 1000);
    run_map_bench(16, 4096, 1, 3, 1000);
    run_map_bench(4, 4096, 8, 3, 1000);
    run_map_bench(4, 4096, 2, 3, 1000);
    run_map_bench(4, 4096, 1, 3, 1000);

    exit(EXIT_SUCCESS);
}