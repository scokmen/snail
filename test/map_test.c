#include <stdbool.h>
#include <stdlib.h>
#include "runner/test_runner.h"
#include "snail.h"

static int* pointer_to_int(int val) {
    int *data = malloc(sizeof (int ));
    *data = val;
    return data;
}

static void map_destructor(const char* key, void* data) {
    if (key == NULL) {
        exit(EXIT_FAILURE);
    }
    if (data == NULL) {
        exit(EXIT_FAILURE);
    }
    free(data);
}

tr_result_t assert_key_value(sn_map_t *map, const char* key, int value) {
    if (!sn_map_has(map, key)) {
        return tr_failed("Expected: True, Found: False, Key: %s", key);
    }

    void* data = sn_map_get(map, key);
    if (data == NULL) {
        return tr_failed("Expected: %d, Found: NULL, Key: %s", value, key);
    }
    if ((*(int *) data) != value) {
        return tr_failed("Expected: %d, Found: %d, Key: %s", value, *(int *) data, key);
    }

    return tr_passed("Passed");
}

tr_result_t crud_operations(SN_UNUSED const void *args) {
    sn_map_t map;
    tr_result_t result;

    ASSERT_ZERO(sn_map_init(&map, 255, map_destructor))
    ASSERT_ZERO(sn_map_set(&map, "key-1", pointer_to_int(1)))
    ASSERT_RESULT(result, assert_key_value(&map, "key-1", 1))

    ASSERT_ZERO(sn_map_set(&map, "key-1", pointer_to_int(2)))
    ASSERT_RESULT(result, assert_key_value(&map, "key-1", 2))

    sn_map_del(&map, "key-1");
    ASSERT_FALSE(sn_map_has(&map, "key-1"))
    ASSERT_NULL(sn_map_get(&map, "key-1"))

    sn_map_destroy(&map);

    return tr_passed("Passed");
}

tr_result_t hash_collisions(SN_UNUSED const void *args) {
    sn_map_t map;

    ASSERT_ZERO(sn_map_init(&map, 1, map_destructor))

    ASSERT_ZERO(sn_map_set(&map, "key-1", pointer_to_int(1)))
    sn_map_del(&map, "key-1");
    ASSERT_ZERO(sn_map_set(&map, "key-2", pointer_to_int(2)))
    ASSERT_ZERO(sn_map_set(&map, "key-3", pointer_to_int(3)))
    sn_map_del(&map, "key-3");
    ASSERT_ZERO(sn_map_set(&map, "key-4", pointer_to_int(4)))
    ASSERT_ZERO(sn_map_set(&map, "key-5", pointer_to_int(5)))
    sn_map_del(&map, "key-4");
    sn_map_del(&map, "key-5");
    ASSERT_ZERO(sn_map_set(&map, "key-6", pointer_to_int(6)))

    ASSERT_FALSE(sn_map_has(&map, "key-1"))
    ASSERT_TRUE(sn_map_has(&map, "key-2"))
    ASSERT_FALSE(sn_map_has(&map, "key-3"))
    ASSERT_FALSE(sn_map_has(&map, "key-4"))
    ASSERT_FALSE(sn_map_has(&map, "key-5"))
    ASSERT_TRUE(sn_map_has(&map, "key-6"))

    return tr_passed("Passed");
}

tr_result_t map_length(SN_UNUSED const void *args) {
    sn_map_t map;

    ASSERT_ZERO(sn_map_init(&map, 128, map_destructor))

    ASSERT_ZERO(sn_map_set(&map, "key-1", pointer_to_int(1)))
    ASSERT_EQ(1, sn_map_len(&map))

    sn_map_del(&map, "key-1");
    ASSERT_EQ(0, sn_map_len(&map))

    ASSERT_ZERO(sn_map_set(&map, "key-2", pointer_to_int(2)))
    ASSERT_ZERO(sn_map_set(&map, "key-3", pointer_to_int(3)))
    ASSERT_EQ(2, sn_map_len(&map))

    sn_map_del(&map, "key-3");
    ASSERT_EQ(1, sn_map_len(&map))

    ASSERT_ZERO(sn_map_set(&map, "key-4", pointer_to_int(4)))
    ASSERT_ZERO(sn_map_set(&map, "key-5", pointer_to_int(5)))
    ASSERT_EQ(3, sn_map_len(&map))

    sn_map_del(&map, "key-4");
    sn_map_del(&map, "key-5");
    ASSERT_EQ(1, sn_map_len(&map))

    ASSERT_ZERO(sn_map_set(&map, "key-6", pointer_to_int(6)))
    ASSERT_EQ(2, sn_map_len(&map))

    sn_map_destroy(&map);

    return tr_passed("Passed");
}

int main(int argc, char **argv) {
    bool result = false;
    tr_suit_t *suit = tr_new_suit("sn_map_t", NULL, NULL);

    tr_add_test_case(suit, "crud_operations", crud_operations);
    tr_add_test_case(suit, "hash_collision", hash_collisions);
    tr_add_test_case(suit, "map_length", map_length);

    result = tr_run_suit(suit, argv, argc);

    return result == true ? EXIT_SUCCESS : EXIT_FAILURE;
}