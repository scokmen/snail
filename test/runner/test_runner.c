#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "test_runner.h"
#include "sn_common.h"

static bool is_case_selected(struct tr_case_s test_case, char **cases, int count) {
    for (int i = 0; i < count; i++) {
        if (strcmp(test_case.name, cases[i]) == 0) {
            return true;
        }
    }
    return false;
}

static tr_result_t create_test_result(bool status, char *fmt, void *data, va_list args) {
    char *message;
    vasprintf(&message, fmt, args);
    tr_result_t result = {
            .status = status,
            .data = data,
            .output = message
    };
    return result;
}

tr_result_t tr_failed(char *fmt, ...) {
    va_list args;
    tr_result_t result;
    va_start(args, fmt);
    result = create_test_result(false, fmt, NULL, args);
    va_end(args);
    return result;
}

tr_result_t tr_failed_with_data(void *data, char *fmt, ...) {
    va_list args;
    tr_result_t result;
    va_start(args, fmt);
    result = create_test_result(false, fmt, data, args);
    va_end(args);
    return result;
}

tr_result_t tr_passed(char *fmt, ...) {
    va_list args;
    tr_result_t result;
    va_start(args, fmt);
    result = create_test_result(true, fmt, NULL, args);
    va_end(args);
    return result;
}

tr_result_t tr_passed_with_data(void *data, char *fmt, ...) {
    va_list args;
    tr_result_t result;
    va_start(args, fmt);
    result = create_test_result(true, fmt, data, args);
    va_end(args);
    return result;
}

tr_suit_t *tr_new_suit(const char *name, tr_tear_up tear_up, tr_tear_down tear_down) {
    tr_suit_t *suit;
    MALLOC_OR_RETURN_NULL(suit, tr_suit_t, 1)
    suit->name = name;
    suit->tear_up = tear_up;
    suit->tear_down = tear_down;
    return suit;
}

void tr_add_test_case(tr_suit_t *suit, const char *name, tr_test_runner runner) {
    struct tr_case_s new_case = {
            .name = name,
            .runner = runner
    };
    if (suit->count < MAX_CASE_IN_SUIT) {
        suit->cases[suit->count++] = new_case;
    }
    // TODO: Handle else case
}

bool tr_run_suit(tr_suit_t *suit, char **cases, int count) {
    void *test_arg = NULL;
    int passes_suits = 0;
    bool suit_result = true;

    if (suit->tear_up != NULL) {
        tr_result_t tear_up_result = suit->tear_up();
        if (tear_up_result.status == false) {
            fprintf(stderr, "Suit: %s, Status: Tear-Up Failed, Error: %s\n", suit->name, tear_up_result.output);
            suit_result = false;
            goto tear_down;
        }
        test_arg = tear_up_result.data;
    }

    for (int i = 0; i < suit->count; i++) {
        tr_result_t test_result;
        struct tr_case_s active_case = suit->cases[i];

        if (is_case_selected(active_case, cases, count)) {
            test_result = active_case.runner(test_arg);
            if (test_result.status == false) {
                fprintf(stderr, "Case: %s, Status: Test Failed, Error: %s\n", active_case.name, test_result.output);
                suit_result = false;
                continue;
            }
            passes_suits++;
        }
    }

    tear_down:
    if (suit->tear_down != NULL) {
        tr_result_t tear_down_result = suit->tear_down(test_arg);
        if (tear_down_result.status == false) {
            fprintf(stderr, "Suit: %s, Status: Tear-Down Failed, Error: %s\n", suit->name, tear_down_result.output);
            suit_result = false;
        }
    }

    return suit_result && (passes_suits > 0);
}