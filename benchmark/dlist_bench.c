#include <stdlib.h>
#include <time.h>
#include "snail.h"

typedef struct {
    int val;
} list_data;

static list_data *list_data_new(int range) {
    list_data *data = malloc(sizeof(list_data));
    if (data == NULL) {
        exit(EXIT_FAILURE);
    }
    data->val = rand() % range;
    return data;
}

static void data_destructor(void *data) {
    free(data);
}

static int comparator(void* given, void* data) {
    return (*(int*)given) - ((list_data*)data)->val;
}

static void run_dlist_bench(int data_range, int num_of_insert, int num_of_iteration, int num_of_random_access) {
    int j;
    void *data;
    sn_dlist_t dlist;
    sn_dlist_init(&dlist, data_destructor);

    for (int iteration = 0; iteration < num_of_iteration; iteration++) {
        list_data **list_items = NULL;

        // Random Insert
        for (int i = 0; i < num_of_insert; i++) {
            if (i % 2 == 0) {
                if (sn_dlist_push(&dlist, list_data_new(data_range)) != 0) {
                    exit(EXIT_FAILURE);
                }
            } else {
                if (sn_dlist_unshift(&dlist, list_data_new(data_range)) != 0) {
                    exit(EXIT_FAILURE);
                }
            }
        }

        list_items = malloc(sizeof (list_data*) * dlist.size);
        if (list_items == NULL) {
            exit(EXIT_FAILURE);
        }

        sn_dlist_collect(&dlist, (void **) list_items, dlist.size);

        // Random Search & Get
        for (int i = 0; i < num_of_random_access; i++) {
            int key = rand() % data_range;
            sn_dlist_has(&dlist, &key, comparator);
        }

        // Random Search & Delete
        for (int i = 0; i < num_of_random_access; i++) {
            int key = rand() % data_range;
            sn_dlist_del(&dlist, &key, comparator);
        }

        // Pop & Unshift
        j = 0;
        while (true) {
            if (j % 2 == 0) {
                data = sn_dlist_pop(&dlist);
            } else {
                data = sn_dlist_shift(&dlist);
            }

            if (data == NULL) {
                break;
            }
            j++;
            free(data);
        }

        free(list_items);
    }

    sn_dlist_destroy(&dlist);
}

int main() {
    srand(time(NULL));

    run_dlist_bench(16, 16, 3, 1000);
    run_dlist_bench(16, 256, 3, 1000);
    run_dlist_bench(16, 1024, 3, 1000);
    run_dlist_bench(16, 4096, 3, 1000);
    run_dlist_bench(64, 16, 3, 1000);
    run_dlist_bench(64, 256, 3, 1000);
    run_dlist_bench(64, 1024, 3, 1000);
    run_dlist_bench(64, 4096, 3, 1000);
    run_dlist_bench(256, 16, 3, 1000);
    run_dlist_bench(256, 256, 3, 1000);
    run_dlist_bench(256, 1024, 3, 1000);
    run_dlist_bench(256, 4096, 3, 1000);
    run_dlist_bench(1024, 16, 3, 1000);
    run_dlist_bench(1024, 256, 3, 1000);
    run_dlist_bench(1024, 1024, 3, 1000);
    run_dlist_bench(1024, 4096, 3, 1000);
    run_dlist_bench(4096, 16, 3, 1000);
    run_dlist_bench(4096, 256, 3, 1000);
    run_dlist_bench(4096, 1024, 3, 1000);
    run_dlist_bench(4096, 4096, 3, 1000);

    exit(EXIT_SUCCESS);
}