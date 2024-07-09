#include <stdbool.h>
#include <stdlib.h>
#include <snail.h>
#include "runner/test_runner.h"

int* generate_int_ptr(int val) {
    int *data = malloc(sizeof (int ));
    *data = val;
    return data;
}

void map_unregister_cb(const char* key, void* data) {
    if (data == NULL) {
        exit(EXIT_FAILURE);
    }
    free(data);
}

tr_test_result assert_key_value(sn_map_t *map, const char* key, int value) {
    if (!sn_map_has(map, key)) {
        return tr_fail("Expected: True, Found: False, Key: %s\n", key);
    }

    void* data = sn_map_get(map, key);
    if (data == NULL) {
        return tr_fail("Expected: %d, Found: NULL, Key: %s\n", value, key);
    }
    if (*(int *) data != value) {
        return tr_fail("Expected: %d, Found: %d, Key: %s\n", value, *(int *) data, key);
    }

    return tr_success("Done!\n");
}

tr_test_result crud_operations(const void *args) {
    sn_map_t *map;
    tr_test_result result;

    if (sn_map_init(&map, 255) != 0) {
        return tr_fail("Cannot initialize sn_map_t\n");
    }

    sn_map_set(map, "key-1", generate_int_ptr(1), map_unregister_cb);
    result = assert_key_value(map, "key-1", 1);
    if (result.status == false) {
        return result;
    }

    sn_map_set(map, "key-1", generate_int_ptr(2), map_unregister_cb);
    result = assert_key_value(map, "key-1", 2);
    if (result.status == false) {
        return result;
    }

    sn_map_del(map, "key-1");
    if (sn_map_has(map, "key-1")) {
        return tr_fail("Expected: False, Found: True, Key: %s\n", "key-1");
    }
    void *data = sn_map_get(map, "key-1");
    if (data != NULL) {
        return tr_fail("Expected: NULL, Found: %d, Key: %s\n", *(int *) data, "key-1");
    }

    sn_map_destroy(map);

    return tr_success("Done!\n");
}

tr_test_result hash_collisions(const void *args) {
    sn_map_t *map;

    if (sn_map_init(&map, 1) != 0) {
        return tr_fail("Cannot initialize sn_map_t\n");
    }

    sn_map_set(map, "key-1", generate_int_ptr(1), map_unregister_cb);
    sn_map_del(map, "key-1");
    sn_map_set(map, "key-2", generate_int_ptr(2), map_unregister_cb);
    sn_map_set(map, "key-3", generate_int_ptr(3), map_unregister_cb);
    sn_map_del(map, "key-3");
    sn_map_set(map, "key-4", generate_int_ptr(4), map_unregister_cb);
    sn_map_set(map, "key-5", generate_int_ptr(5), map_unregister_cb);
    sn_map_del(map, "key-4");
    sn_map_del(map, "key-5");
    sn_map_set(map, "key-6", generate_int_ptr(6), map_unregister_cb);

    if (sn_map_has(map, "key-1")) {
        return tr_fail("Expected: False, Found: True, Key: %s\n", "key-1");
    }

    if (!sn_map_has(map, "key-2")) {
        return tr_fail("Expected: True, Found: False, Key: %s\n", "key-2");
    }

    if (sn_map_has(map, "key-3")) {
        return tr_fail("Expected: False, Found: True, Key: %s\n", "key-3");
    }

    if (sn_map_has(map, "key-4")) {
        return tr_fail("Expected: False, Found: True, Key: %s\n", "key-4");
    }

    if (sn_map_has(map, "key-5")) {
        return tr_fail("Expected: False, Found: True, Key: %s\n", "key-5");
    }

    if (!sn_map_has(map, "key-6")) {
        return tr_fail("Expected: False, Found: True, Key: %s\n", "key-6");
    }

    return tr_success("Done!\n");
}

tr_test_result map_length(const void *args) {
    sn_map_t *map;

    if (sn_map_init(&map, 128) != 0) {
        return tr_fail("Cannot initialize sn_map_t\n");
    }

    sn_map_set(map, "key-1", generate_int_ptr(1), map_unregister_cb);
    if (sn_map_length(map) != 1) {
        return tr_fail("Expected: %d, Found: %zu\n", 1, sn_map_length(map));
    }

    sn_map_del(map, "key-1");
    if (sn_map_length(map) != 0) {
        return tr_fail("Expected: %d, Found: %zu\n", 0, sn_map_length(map));
    }

    sn_map_set(map, "key-2", generate_int_ptr(2), map_unregister_cb);
    sn_map_set(map, "key-3", generate_int_ptr(3), map_unregister_cb);
    if (sn_map_length(map) != 2) {
        return tr_fail("Expected: %d, Found: %zu\n", 2, sn_map_length(map));
    }

    sn_map_del(map, "key-3");
    if (sn_map_length(map) != 1) {
        return tr_fail("Expected: %d, Found: %zu\n", 1, sn_map_length(map));
    }

    sn_map_set(map, "key-4", generate_int_ptr(4), map_unregister_cb);
    sn_map_set(map, "key-5", generate_int_ptr(5), map_unregister_cb);
    if (sn_map_length(map) != 3) {
        return tr_fail("Expected: %d, Found: %zu\n", 3, sn_map_length(map));
    }

    sn_map_del(map, "key-4");
    sn_map_del(map, "key-5");
    if (sn_map_length(map) != 1) {
        return tr_fail("Expected: %d, Found: %zu\n", 1, sn_map_length(map));
    }

    sn_map_set(map, "key-6", generate_int_ptr(6), map_unregister_cb);
    if (sn_map_length(map) != 2) {
        return tr_fail("Expected: %d, Found: %zu\n", 2, sn_map_length(map));
    }

    return tr_success("Done!\n");
}

int main(int argc, char **argv) {
    bool result = false;
    tr_test_suit *suit = tr_new_suit("SN_MAP SUIT");

    tr_add_test_case(suit,
                     tr_new_case("Simple CRUD Operations", NULL, crud_operations, NULL));
    tr_add_test_case(suit,
                     tr_new_case("Hash Collision", NULL, hash_collisions, NULL));
    tr_add_test_case(suit,
                     tr_new_case("Map Length", NULL, map_length, NULL));

    result = tr_run_suit(suit);

    return result == true ? EXIT_SUCCESS : EXIT_FAILURE;
}