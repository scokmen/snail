#ifndef SNAIL_TEST_RUNNER_H
#define SNAIL_TEST_RUNNER_H

#include <stdio.h>
#include <stdbool.h>

#define MAX_CASE_IN_SUIT (32)

typedef struct test_result {
    bool status;
    const char *output;
} test_result;

typedef test_result (*test_callback)(const void *args);

typedef struct test_case {
    const void *args;
    const char *name;
    test_callback callback;
} test_case;

typedef struct test_suit {
    int length;
    const char *name;
    test_case *cases[MAX_CASE_IN_SUIT];
} test_suit;

test_result test_result_new(bool result, const char *output);

test_case *test_case_new(const char *name, const void *args, test_callback callback);

test_suit *test_suit_new(const char *name);

void test_suit_add(test_suit *suit, test_case *new_case);

bool test_suit_run(test_suit *suit);

#endif //SNAIL_TEST_RUNNER_H
