#include <stdlib.h>
#include "wait_group.h"

WaitGroup *wait_group_init()
{
    WaitGroup *wg = malloc(sizeof(WaitGroup));
    if (wg == NULL)
        return NULL;

    if (pthread_mutex_init(&wg->mu, NULL) != 0)
    {
        free(wg);
        return NULL;
    }

    if (pthread_cond_init(&wg->done, NULL) != 0)
    {
        pthread_mutex_destroy(&wg->mu);
        free(wg);
    }

    wg->counter = 0;

    return wg;
}

void wait_group_add(WaitGroup *wg, int n)
{
    pthread_mutex_lock(&wg->mu);
    wg->counter += n;
    pthread_mutex_unlock(&wg->mu);
}

void wait_group_done(WaitGroup *wg)
{
    pthread_mutex_lock(&wg->mu);
    wg->counter--;
    if (wg->counter == 0)
        pthread_cond_signal(&wg->done);
    pthread_mutex_unlock(&wg->mu);
}

void wait_group_wait(WaitGroup *wg)
{
    pthread_mutex_lock(&wg->mu);

    while (wg->counter > 0)
        pthread_cond_wait(&wg->done, &wg->mu);

    pthread_mutex_unlock(&wg->mu);
}

void wait_group_free(WaitGroup *wg)
{
    pthread_mutex_destroy(&wg->mu);
    pthread_cond_destroy(&wg->done);
    free(wg);
}