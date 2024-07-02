#ifndef SNAIL_TEST_RUNNER_H
#define SNAIL_TEST_RUNNER_H

#include <stdio.h>
#include <stdbool.h>

#if (defined(__GCC__) || defined(__clang__)) && defined(__has_attribute)
#  if __has_attribute (format)
#    define TR_FORMAT(FROM) __attribute__ ((format (printf, (FROM), (FROM + 1))))
#  else
#    define TR_FORMAT
#  endif
#else
#  define TR_FORMAT
#endif

#define MAX_CASE_IN_SUIT (32)

typedef struct tr_test_result {
    bool status;
    void *data;
    const char *output;
} tr_test_result;

typedef tr_test_result (*tr_tear_up_callback)();

typedef tr_test_result (*tr_test_runner)(const void *args);

typedef tr_test_result (*tr_tear_down_callback)(const void *args);

typedef struct tr_test_case {
    const char *name;
    tr_tear_up_callback tear_up;
    tr_test_runner runner;
    tr_tear_down_callback tear_down;
} tr_test_case;

typedef struct tr_test_suit {
    int length;
    const char *name;
    tr_test_case *cases[MAX_CASE_IN_SUIT];
} tr_test_suit;

tr_test_result tr_success(char *fmt, ...) TR_FORMAT(1);

tr_test_result tr_fail(char *fmt, ...) TR_FORMAT(1);

tr_test_result tr_success_ext(void *data, char *fmt, ...) TR_FORMAT(2);

tr_test_result tr_fail_ext(void *data, char *fmt, ...) TR_FORMAT(2);

tr_test_suit *tr_new_suit(const char *name);

tr_test_case *tr_new_case(const char *name, tr_tear_up_callback tear_up, tr_test_runner callback, tr_tear_down_callback tear_down);

void tr_add_test_case(tr_test_suit *suit, tr_test_case *new_case);

bool tr_run_suit(tr_test_suit *suit);

#endif //SNAIL_TEST_RUNNER_H
