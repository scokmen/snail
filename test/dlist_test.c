#include <stdbool.h>
#include <stdlib.h>
#include <snail.h>
#include "runner/test_runner.h"

static int *pointer_to_int(int val) {
    int *data = malloc(sizeof(int));
    *data = val;
    return data;
}

static void data_destructor(void *data) {
    if (data == NULL) {
        exit(EXIT_FAILURE);
    }
    free(data);
}

static int comparator(void* given, void* data) {
    return (*(int*)given) - *((int*)data);
}

static tr_test_result assert_dlist_values(sn_dlist_t *list, int* expectations, size_t limit) {
    int **items = malloc(sizeof (int) * limit);
    tr_test_result test_result = tr_success("Passed");
    sn_dlist_collect(list, (void **) items, limit);
    for (int i = 0; i < limit; i++) {
        if ((*items[i]) != expectations[i]) {
            test_result = tr_fail("Expected: %d, Found: %d, Index: %d", expectations[i], *items[i], i);
            break;
        }
    }
    free(items);

    if (limit != list->size) {
        test_result = tr_fail("Expected: %zu, Found: %zu", limit, list->size);
    }

    return test_result;
}

tr_test_result first_and_last(const void *args) {
    sn_dlist_t list;

    sn_dlist_init(&list, data_destructor);

    ASSERT_SUCCESS_CODE(sn_dlist_push(&list, pointer_to_int(1)))
    ASSERT_SUCCESS_CODE(sn_dlist_push(&list, pointer_to_int(2)))
    ASSERT_SUCCESS_CODE(sn_dlist_push(&list, pointer_to_int(3)))

    ASSERT_EQ(1, *(int*)sn_dlist_first(&list))
    ASSERT_EQ(3, *(int*)sn_dlist_last(&list))

    sn_dlist_destroy(&list);

    return tr_success("Passed");
}

tr_test_result insertion_and_deletion(const void *args) {
    void *data;
    sn_dlist_t list;
    tr_test_result test_result;

    sn_dlist_init(&list, data_destructor);

    ASSERT_SUCCESS_CODE(sn_dlist_push(&list, pointer_to_int(1)))
    ASSERT_SUCCESS_CODE(sn_dlist_push(&list, pointer_to_int(2)))
    ASSERT_SUCCESS_CODE(sn_dlist_push(&list, pointer_to_int(3)))
    ASSERT_RESULT(test_result, assert_dlist_values(&list, (int[]){1,2,3}, 3))

    data = sn_dlist_pop(&list);
    ASSERT_RESULT(test_result, assert_dlist_values(&list, (int[]){1,2}, 2))
    free(data);

    data = sn_dlist_pop(&list);
    ASSERT_RESULT(test_result, assert_dlist_values(&list, (int[]){1}, 1))
    free(data);

    data = sn_dlist_pop(&list);
    ASSERT_RESULT(test_result, assert_dlist_values(&list, (int[]){}, 0))
    free(data);

    ASSERT_NULL(sn_dlist_pop(&list))

    ASSERT_SUCCESS_CODE(sn_dlist_unshift(&list, pointer_to_int(4)))
    ASSERT_RESULT(test_result, assert_dlist_values(&list, (int[]){4}, 1))

    ASSERT_SUCCESS_CODE(sn_dlist_unshift(&list, pointer_to_int(5)))
    ASSERT_RESULT(test_result, assert_dlist_values(&list, (int[]){5, 4}, 2))

    data = sn_dlist_shift(&list);
    ASSERT_RESULT(test_result, assert_dlist_values(&list, (int[]){4}, 1))
    free(data);

    data = sn_dlist_shift(&list);
    ASSERT_RESULT(test_result, assert_dlist_values(&list, (int[]){0}, 0))
    free(data);

    ASSERT_NULL(sn_dlist_shift(&list))

    sn_dlist_destroy(&list);

    return tr_success("Passed");
}

tr_test_result search_and_get(const void *args) {
    int key1 = 1, key2 = 10;
    sn_dlist_t list;

    sn_dlist_init(&list, data_destructor);

    ASSERT_SUCCESS_CODE( sn_dlist_push(&list, pointer_to_int(3)))
    ASSERT_SUCCESS_CODE(sn_dlist_push(&list, pointer_to_int(1)))
    ASSERT_SUCCESS_CODE(sn_dlist_push(&list, pointer_to_int(2)))

    ASSERT_EQ(1, *(int*)sn_dlist_get(&list, &key1, comparator))
    ASSERT_TRUE(sn_dlist_has(&list, &key1, comparator))
    ASSERT_NULL(sn_dlist_get(&list, &key2, comparator))
    ASSERT_FALSE(sn_dlist_has(&list, &key2, comparator))

    sn_dlist_destroy(&list);

    return tr_success("Passed");
}

tr_test_result search_and_del(const void *args) {
    int key1 = 1, key2 = 10;
    sn_dlist_t list;

    sn_dlist_init(&list, data_destructor);

    ASSERT_SUCCESS_CODE(sn_dlist_push(&list, pointer_to_int(1)))
    ASSERT_SUCCESS_CODE(sn_dlist_push(&list, pointer_to_int(2)))
    ASSERT_SUCCESS_CODE(sn_dlist_push(&list, pointer_to_int(1)))

    ASSERT_TRUE(sn_dlist_del(&list, &key1, comparator))
    ASSERT_TRUE(sn_dlist_del(&list, &key1, comparator))
    ASSERT_FALSE(sn_dlist_del(&list, &key1, comparator))
    ASSERT_FALSE(sn_dlist_del(&list, &key2, comparator))
    ASSERT_EQ(1, sn_dlist_len(&list))

    sn_dlist_destroy(&list);

    return tr_success("Passed");
}

int main(int argc, char **argv) {
    bool result = false;
    tr_test_suit *suit = tr_new_suit("sn_dlist_t");

    tr_add_test_case(suit,
                     tr_new_case("first_and_last", NULL, first_and_last, NULL));

    tr_add_test_case(suit,
                     tr_new_case("insertion_and_deletion", NULL, insertion_and_deletion, NULL));

    tr_add_test_case(suit,
                     tr_new_case("search_and_get", NULL, search_and_get, NULL));

    tr_add_test_case(suit,
                     tr_new_case("search_and_del", NULL, search_and_del, NULL));

    result = tr_run_suit(suit, argv, argc);

    return result == true ? EXIT_SUCCESS : EXIT_FAILURE;
}