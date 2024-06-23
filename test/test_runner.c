#include <stdlib.h>
#include "test_runner.h"

test_result test_result_new(bool status, const char *output) {
    test_result result = {.status = status, .output = output};
    return result;
}

test_case *test_case_new(const char *name, const void *args, test_callback callback) {
    test_case *test = malloc(sizeof(test_case));
    test->name = name;
    test->args = args;
    test->callback = callback;
    return test;
}

test_suit *test_suit_new(const char *name) {
    test_suit *suit = malloc(sizeof(test_suit));
    suit->name = name;
    return suit;
}

void test_suit_add(test_suit *suit, test_case *new_case) {
    if (suit->length < MAX_CASE_IN_SUIT) {
        suit->cases[suit->length] = new_case;
        suit->length++;
    }
}

bool test_suit_run(test_suit *suit) {
    bool suit_result = true;
    fprintf(stdout, "TEST SUIT: %s\n", suit->name);
    for (int i = 0; i < suit->length; i++) {
        test_result result;
        test_case *active_case = suit->cases[i];
        result = active_case->callback(active_case->args);
        if (result.status == false) {
            fprintf(stderr, " %d: CASE: %s\nSTATUS: FAILED\nERROR:%s\n", i + 1, active_case->name, result.output);
            suit_result = false;
        } else {
            fprintf(stdout, " %d: CASE: %s\nSTATUS: PASSED\n", i + 1, active_case->name);
        }
    }

    return suit_result;
}