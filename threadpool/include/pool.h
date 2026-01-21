#ifndef POOL_H
#define POOL_H

#include <stddef.h>
#include <pthread.h>
#include "channel.h"
#include "wait_group.h"

struct task
{
    void (*fn)(void *p);
    void *p;
};

typedef struct
{
    size_t size;
    pthread_t *threads;
    Channel *ch;
    WaitGroup *wg;
    WaitGroup *init;
} Pool;

Pool *pool_init(size_t size);
void pool_submit(Pool *pool, void (*fn)(void *p), void *p);
void pool_shutdown(Pool *pool);

#endif