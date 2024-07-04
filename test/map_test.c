#include <stdbool.h>
#include <stdlib.h>
#include "snail.h"
#include "runner/test_runner.h"

int* generate_int_ptr(int val) {
    int *data = malloc(sizeof (int ));
    *data = val;
    return data;
}

void destroy_int_ptr(void* ptr) {
    if (ptr != NULL){
        free(ptr);
    }
}

tr_test_result validate_key_value(sn_map_t *map, const char* key, int value) {
    if (sn_map_has(map, key) == false) {
        return tr_fail("Expected: True, Found: False");
    }

    void* data = sn_map_get(map, key);
    if (data == NULL) {
        return tr_fail("Expected: %d, Found: NULL", value);
    }
    if (*(int *) data != value) {
        return tr_fail("Expected: %d, Found: %d", value, *(int *) data);
    }
    return tr_success("Done");
}

tr_test_result crud_operations(const void *args) {
    sn_map_t *map = sn_map_init((uint8_t)255);
    tr_test_result result;

    sn_map_set(map, "key-1", generate_int_ptr(1), destroy_int_ptr);
    result = validate_key_value(map, "key-1", 1);
    if (result.status == false) {
        return result;
    }

    sn_map_set(map, "key-1", generate_int_ptr(2), destroy_int_ptr);
    result = validate_key_value(map, "key-1", 2);
    if (result.status == false) {
        return result;
    }

    sn_map_del(map, "key-1");
    if (sn_map_has(map, "key-1") == true) {
        return tr_fail("Expected: False, Found: True");
    }
    if (sn_map_get(map, "key-1") != NULL) {
        return tr_fail("Expected: NULL, Found: NON_NULL");
    }

    sn_map_destroy(map);

    return tr_success("Done");
}

int main() {
    bool result = false;
    tr_test_suit *suit = tr_new_suit("SN_MAP SUIT");

    tr_add_test_case(suit,
                     tr_new_case("Simple CRUD Operations", NULL, crud_operations, NULL));

    result = tr_run_suit(suit);

    return result == true ? EXIT_SUCCESS : EXIT_FAILURE;
}