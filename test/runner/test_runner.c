#include <stdlib.h>
#include <stdarg.h>
#include "test_runner.h"

test_result test_result_new(bool status, char *fmt, ...) {
    va_list args;
    test_result result;
    va_start(args, fmt);
    result = test_result_with_arg(status, NULL, fmt, args);
    va_end(args);
    return result;
}

test_result test_result_with_arg(bool status, void *data, char *fmt, ...) {
    va_list args;
    char *message = malloc(sizeof(char) * TEST_OUT_MAX_LEN);
    va_start(args, fmt);
    sprintf(message, fmt, args);
    va_end(args);
    test_result result = {
            .status = status,
            .data = data,
            .output = message
    };
    return result;
}

test_case *test_case_new(const char *name, test_tear_up tear_up, test_callback callback, test_tear_down tear_down) {
    test_case *test = malloc(sizeof(test_case));
    test->name = name;
    test->tear_up = tear_up;
    test->callback = callback;
    test->tear_down = tear_down;
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
    // TODO: Handle else case
}

bool test_suit_run(test_suit *suit) {
    bool suit_result = true;
    fprintf(stdout, "RUNNING TEST SUIT: %s\n", suit->name);
    for (int i = 0; i < suit->length; i++) {
        test_result result;
        test_case *active_case = suit->cases[i];
        void *arg = NULL;
        if (active_case->tear_up != NULL) {
            test_result tear_up_result = active_case->tear_up();
            if (tear_up_result.status == false) {
                fprintf(stderr, " %d: CASE: %s\nSTATUS: TEAR-UP FAILED\nERROR:%s\n", i + 1, active_case->name, tear_up_result.output);
                suit_result = false;
                continue;
            }
            arg = tear_up_result.data;
        }
        result = active_case->callback(arg);
        if (result.status == false) {
            fprintf(stderr, " %d: CASE: %s\nSTATUS: FAILED\nERROR:%s\n", i + 1, active_case->name, result.output);
            suit_result = false;
        } else {
            fprintf(stdout, " %d: CASE: %s\nSTATUS: PASSED\n", i + 1, active_case->name);
        }
        if (active_case->tear_down != NULL) {
            test_result tear_down_result = active_case->tear_down(arg);
            if (tear_down_result.status == false) {
                fprintf(stderr, " %d: CASE: %s\nSTATUS: TEAR-DOWN FAILED\nERROR:%s\n", i + 1, active_case->name, tear_down_result.output);
                suit_result = false;
            }
        }
    }

    return suit_result;
}