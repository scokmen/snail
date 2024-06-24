#ifndef SNAIL_TEST_RUNNER_H
#define SNAIL_TEST_RUNNER_H

#include <stdio.h>
#include <stdbool.h>

#if defined __has_attribute
#  if __has_attribute (format)
#    define SN_FORMAT_FN(FROM) __attribute__ ((format (printf, (FROM), (FROM + 1))))
#  endif
#endif

#define MAX_CASE_IN_SUIT (32)
#define TEST_OUT_MAX_LEN (256)

typedef struct test_result {
    bool status;
    void *data;
    const char *output;
} test_result;

typedef test_result (*test_tear_up)();

typedef test_result (*test_callback)(const void *args);

typedef test_result (*test_tear_down)(const void *args);

typedef struct test_case {
    const char *name;
    test_tear_up tear_up;
    test_callback callback;
    test_tear_down tear_down;
} test_case;

typedef struct test_suit {
    int length;
    const char *name;
    test_case *cases[MAX_CASE_IN_SUIT];
} test_suit;

test_result test_result_new(bool status, char *fmt, ...) SN_FORMAT_FN(2);

test_result test_result_with_arg(bool status, void *data, char *fmt, ...) SN_FORMAT_FN(3);

test_case *test_case_new(const char *name, test_tear_up tear_up, test_callback callback, test_tear_down tear_down);

test_suit *test_suit_new(const char *name);

void test_suit_add(test_suit *suit, test_case *new_case);

bool test_suit_run(test_suit *suit);

#endif //SNAIL_TEST_RUNNER_H
