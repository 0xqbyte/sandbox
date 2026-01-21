#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include "pool.h"

static void *worker(void *p);
static struct task *task_init(void (*fn)(void *p), void *p);
static void task_free(struct task *t);
void *pool_free(Pool *pool);

Pool *pool_init(size_t size)
{
    Pool *pool = malloc(sizeof(Pool));
    if (!pool)
        return NULL;

    pool->size = size;

    pool->threads = calloc(pool->size, sizeof(pthread_t));
    if (!pool->threads)
        return pool_free(pool);

    pool->ch = channel_init();
    if (!pool->ch)
        return pool_free(pool);

    pool->wg = wait_group_init();
    if (!pool->wg)
        return pool_free(pool);

    pool->init = wait_group_init();
    if (!pool->init)
        return pool_free(pool);

    wait_group_add(pool->wg, size);
    wait_group_add(pool->init, pool->size);

    for (size_t i = 0; i < pool->size; i++)
        if (pthread_create(&pool->threads[i], NULL, worker, pool) != 0)
            return pool_free(pool);

    wait_group_wait(pool->init);
    wait_group_free(pool->init);
    pool->init = NULL;

    return pool;
}

void pool_submit(Pool *pool, void (*fn)(void *p), void *p)
{
    struct task *t = task_init(fn, p);
    if (!t)
        return;

    channel_send(pool->ch, t);
}

void pool_shutdown(Pool *pool)
{
    channel_close(pool->ch);

    wait_group_wait(pool->wg);

    for (size_t i = 0; i < pool->size; i++)
        pthread_join(pool->threads[i], NULL);

    pool_free(pool);
}

void *pool_free(Pool *pool)
{
    if (pool->init)
        wait_group_free(pool->init);
    wait_group_free(pool->wg);
    channel_free(pool->ch);
    free(pool->threads);
    free(pool);
    return NULL;
}

static void *worker(void *p)
{
    Pool *pool = p;
    struct task *t;

    wait_group_done(pool->init);

    while ((t = channel_recv(pool->ch)))
    {
        t->fn(t->p);
        task_free(t);
    }

    wait_group_done(pool->wg);

    return NULL;
}

static struct task *task_init(void (*fn)(void *p), void *p)
{
    struct task *t = malloc(sizeof(struct task));
    if (!t)
        return NULL;

    t->fn = fn;
    t->p = p;

    return t;
}

static void task_free(struct task *t)
{
    free(t);
}