#ifndef SNAIL_TEST_RUNNER_H
#define SNAIL_TEST_RUNNER_H

#include <stdio.h>
#include <stdbool.h>
#include "snail.h"

#define MAX_CASE_IN_SUIT (32)

#define ASSERT_RESULT(RESULT, EXP) RESULT = EXP;                                                                            \
                                   if (!EXP.status) {                                                                       \
                                       return RESULT;                                                                       \
                                   }                                                                                        \

#define ASSERT_EQ(EXP, ACT)        if (EXP != ACT) {                                                                        \
                                       return tr_failed("Expected: Equal, Found: Not Equal! at %s:%d", __FILE__, __LINE__); \
                                   }                                                                                        \

#define ASSERT_ONE(EXP)            ASSERT_EQ(EXP, 1)
#define ASSERT_ZERO(EXP)           ASSERT_EQ(EXP, 0)
#define ASSERT_TRUE(EXP)           ASSERT_ONE(EXP)
#define ASSERT_FALSE(EXP)          ASSERT_ZERO(EXP)
#define ASSERT_NULL(EXP)           ASSERT_EQ(EXP, NULL)

typedef struct tr_result_s tr_result_t;
typedef struct tr_suit_s tr_suit_t;

typedef tr_result_t (*tr_tear_up)  ();
typedef tr_result_t (*tr_test_runner)(const void *args);
typedef tr_result_t (*tr_tear_down)(const void *args);

struct tr_result_s {
    bool status;
    void *data;
    const char *output;
};

struct tr_case_s {
    const char *name;
    tr_test_runner runner;
};

struct tr_suit_s {
    int count;
    const char *name;
    tr_tear_up tear_up;
    tr_tear_down tear_down;
    struct tr_case_s cases[MAX_CASE_IN_SUIT];
};

SN_FORMAT(1)
tr_result_t tr_failed(char *fmt, ...);

SN_FORMAT(2)
tr_result_t tr_failed_with_data(void *data, char *fmt, ...);

SN_FORMAT(1)
tr_result_t tr_passed(char *fmt, ...);

SN_FORMAT(2)
tr_result_t tr_passed_with_data(void *data, char *fmt, ...);

tr_suit_t *tr_new_suit(const char *name, tr_tear_up tear_up, tr_tear_down tear_down);

void tr_add_test_case(tr_suit_t *suit, const char *name, tr_test_runner runner);

bool tr_run_suit(tr_suit_t *suit, char **suits, int count);

#endif //SNAIL_TEST_RUNNER_H
