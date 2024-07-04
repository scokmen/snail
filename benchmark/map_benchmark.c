#include <snail.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>

typedef struct {
    int val;
} map_data;

map_data *generate_node(int val) {
    map_data * data = malloc(sizeof (map_data));
    data->val = val;
    return data;
}

void unregister_cb(void *data) {
    free(data);
}

const char *generate_random_key(int length) {
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

const char* pick_random_key(const char** keys, int count) {
    return keys[rand() % count];
}

void run_sn_map_memcheck(int16_t bucket_size, int count, int key_length, int num_of_iteration, int num_of_random_access) {
    const char** keys = calloc(count, sizeof (char*));
    if (keys == NULL) {
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < count; i++) {
        keys[i] = generate_random_key(key_length);
        if (keys[i] == NULL) {
            exit(EXIT_FAILURE);
        }
    }

    sn_map_t *map = sn_map_init(bucket_size);

    for (int i = 0; i < count; i++) {
        bool result = sn_map_set(map, keys[i], generate_node(1), unregister_cb);
        if (!result) {
            exit(EXIT_FAILURE);
        }
    }

    for (int iteration = 0; iteration < num_of_iteration; iteration++) {
        // Random Access
        for (int i = 0; i < num_of_random_access; i++) {
            const char* key = pick_random_key(keys, count);
            if (sn_map_has(map, key)) {
                void* data = sn_map_get(map, key);
                if ((*(int*)data) != 1) {
                    exit(EXIT_FAILURE);
                }
            }
        }

        // Random Overwrite
        for (int i = 0; i < num_of_random_access; i++) {
            const char* key = pick_random_key(keys, count);
            if (!sn_map_set(map, key, generate_node(1), unregister_cb)) {
                exit(EXIT_FAILURE);
            }
        }

        // Random Delete
        for (int i = 0; i < 10000; i++) {
            const char* key = pick_random_key(keys, count);
            if (!sn_map_set(map, key, generate_node(1), unregister_cb)) {
                exit(EXIT_FAILURE);
            }
        }
    }

    for (int i = 0; i < count; i++) {
        free((char*)keys[i]);
    }

    free(keys);

    sn_map_destroy(map);
}

int main(int argc, char **argv) {
    srand(time(NULL));
    run_sn_map_memcheck(1024, 4096, 8, 3, 1000);
    run_sn_map_memcheck(1024, 4096, 2, 3, 1000);
    run_sn_map_memcheck(1024, 4096, 1, 3, 1000);
    run_sn_map_memcheck(128,  4096, 8, 3, 1000);
    run_sn_map_memcheck(128,  4096, 2, 3, 1000);
    run_sn_map_memcheck(128,  4096, 1, 3, 1000);
    run_sn_map_memcheck(16,   4096, 8, 3, 1000);
    run_sn_map_memcheck(16,   4096, 2, 3, 1000);
    run_sn_map_memcheck(16,   4096, 1, 3, 1000);
    run_sn_map_memcheck(4,    4096, 8, 3, 1000);
    run_sn_map_memcheck(4,    4096, 2, 3, 1000);
    run_sn_map_memcheck(4,    4096, 1, 3, 1000);
    exit(EXIT_SUCCESS);
}