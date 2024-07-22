#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "test_runner.h"

static bool is_case_selected(tr_test_case *test_case, char **cases, int count) {
    for (int i = 0; i < count; i++) {
        if (strcmp(test_case->name, cases[i]) == 0) {
            return true;
        }
    }
    return false;
}

static tr_test_result create_test_result(bool status, char *fmt, void *data, va_list args) {
    char *message;
    vasprintf(&message, fmt, args);
    tr_test_result result = {
            .status = status,
            .data = data,
            .output = message
    };
    return result;
}

tr_test_result tr_fail(char *fmt, ...) {
    va_list args;
    tr_test_result result;
    va_start(args, fmt);
    result = create_test_result(false, fmt, NULL, args);
    va_end(args);
    return result;
}

tr_test_result tr_fail_ext(void *data, char *fmt, ...) {
    va_list args;
    tr_test_result result;
    va_start(args, fmt);
    result = create_test_result(false, fmt, data, args);
    va_end(args);
    return result;
}

tr_test_result tr_success(char *fmt, ...) {
    va_list args;
    tr_test_result result;
    va_start(args, fmt);
    result = create_test_result(true, fmt, NULL, args);
    va_end(args);
    return result;
}

tr_test_result tr_success_ext(void *data, char *fmt, ...) {
    va_list args;
    tr_test_result result;
    va_start(args, fmt);
    result = create_test_result(true, fmt, data, args);
    va_end(args);
    return result;
}

tr_test_suit *tr_new_suit(const char *name) {
    tr_test_suit *suit = malloc(sizeof(tr_test_suit));
    suit->name = name;
    return suit;
}

tr_test_case *tr_new_case(const char *name, tr_tear_up_callback tear_up, tr_test_runner callback, tr_tear_down_callback tear_down) {
    tr_test_case *test = malloc(sizeof(tr_test_case));
    test->name = name;
    test->tear_up = tear_up;
    test->runner = callback;
    test->tear_down = tear_down;
    return test;
}

void tr_add_test_case(tr_test_suit *suit, tr_test_case *new_case) {
    if (suit->count < MAX_CASE_IN_SUIT) {
        suit->cases[suit->count++] = new_case;
    }
    // TODO: Handle else case
}

bool tr_run_suit(tr_test_suit *suit, char **cases, int count) {
    int passes_suits = 0;
    bool suit_result = true;

    for (int i = 0; i < suit->count; i++) {
        bool passed = true;
        tr_test_result result;
        tr_test_case *active_case = suit->cases[i];
        if (!is_case_selected(active_case, cases, count)) {
            continue;
        }
        void *arg = NULL;
        if (active_case->tear_up != NULL) {
            tr_test_result tear_up_result = active_case->tear_up();
            if (tear_up_result.status == false) {
                fprintf(stderr, "Case: %s, Status: Tear-Up Failed, Error: %s\n", active_case->name, tear_up_result.output);
                passed = false;
                suit_result = false;
            }
            arg = tear_up_result.data;
        }
        if (passed) {
            result = active_case->runner(arg);
            if (result.status == false) {
                fprintf(stderr, "Case: %s, Status: Test Failed, Error: %s\n", active_case->name, result.output);
                passed = false;
                suit_result = false;
            }
        }
        if (active_case->tear_down != NULL) {
            tr_test_result tear_down_result = active_case->tear_down(arg);
            if (tear_down_result.status == false) {
                fprintf(stderr, "Case: %s, Status: Tear-Down Failed, Error: %s\n", active_case->name, tear_down_result.output);
                passed = false;
                suit_result = false;
                continue;
            }
        }

        if (passed) {
            passes_suits++;
        }
    }

    return suit_result && (passes_suits > 0);
}