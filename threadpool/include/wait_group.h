#ifndef WAITGROUP_H
#define WAITGROUP_H

#include <stddef.h>
#include <pthread.h>

typedef struct
{
    size_t counter;
    pthread_mutex_t mu;
    pthread_cond_t done;
} WaitGroup;

WaitGroup *wait_group_init();
void wait_group_add(WaitGroup *wg, int n);
void wait_group_done(WaitGroup *wg);
void wait_group_wait(WaitGroup *wg);
void wait_group_free(WaitGroup *wg);

#endif