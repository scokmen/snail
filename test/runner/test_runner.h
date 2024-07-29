#ifndef SNAIL_TEST_RUNNER_H
#define SNAIL_TEST_RUNNER_H

#include <stdio.h>
#include <stdbool.h>
#include "snail.h"

#define MAX_CASE_IN_SUIT (32)

#define ASSERT_RESULT(RESULT, EXP) RESULT = EXP;                                                                          \
                                   if (!EXP.status) {                                                                     \
                                       return RESULT;                                                                     \
                                   }                                                                                      \

#define ASSERT_EQ(EXP, ACT)        if (EXP != ACT) {                                                                      \
                                       return tr_fail("Expected: Equal, Found: Not Equal! at %s:%d", __FILE__, __LINE__); \
                                   }                                                                                      \

#define ASSERT_ONE(EXP)            ASSERT_EQ(EXP, 1)
#define ASSERT_ZERO(EXP)           ASSERT_EQ(EXP, 0)
#define ASSERT_TRUE(EXP)           ASSERT_ONE(EXP)
#define ASSERT_FALSE(EXP)          ASSERT_ZERO(EXP)
#define ASSERT_NULL(EXP)           ASSERT_EQ(EXP, NULL)

typedef struct tr_test_result {
    bool status;
    void *data;
    const char *output;
} tr_test_result;

typedef tr_test_result (*tr_tear_up_callback)();

typedef tr_test_result (*tr_test_runner)(const void *args);

typedef tr_test_result (*tr_tear_down_callback)(const void *args);

typedef struct {
    const char *name;
    tr_tear_up_callback tear_up;
    tr_test_runner runner;
    tr_tear_down_callback tear_down;
} tr_test_case;

typedef struct {
    int count;
    const char *name;
    tr_test_case *cases[MAX_CASE_IN_SUIT];
} tr_test_suit;

SN_FORMAT(1)
tr_test_result tr_fail(char *fmt, ...);

SN_FORMAT(2)
tr_test_result tr_fail_ext(void *data, char *fmt, ...);

SN_FORMAT(1)
tr_test_result tr_success(char *fmt, ...);

SN_FORMAT(2)
tr_test_result tr_success_ext(void *data, char *fmt, ...);

tr_test_suit *tr_new_suit(const char *name);

tr_test_case *tr_new_case(const char *name, tr_tear_up_callback tear_up, tr_test_runner callback, tr_tear_down_callback tear_down);

void tr_add_test_case(tr_test_suit *suit, tr_test_case *new_case);

bool tr_run_suit(tr_test_suit *suit, char **suits, int count);

#endif //SNAIL_TEST_RUNNER_H
