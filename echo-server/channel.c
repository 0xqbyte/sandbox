#include <stdlib.h>
#include <pthread.h>
#include "channel.h"
#include "queue.h"

Channel *channel_init()
{
    Channel *chan = (Channel *)malloc(sizeof(Channel));
    if (chan == NULL)
        return NULL;

    chan->queue = queue_init();
    if (chan->queue == NULL)
    {
        free(chan);
        return NULL;
    }

    if (pthread_mutex_init(&chan->mu, NULL) != 0)
    {
        queue_free(chan->queue);
        free(chan);
        return NULL;
    }

    if (pthread_cond_init(&chan->cond, NULL) != 0)
    {
        pthread_mutex_destroy(&chan->mu);
        queue_free(chan->queue);
        free(chan);
        return NULL;
    }

    chan->closed = false;

    return chan;
}

void channel_send(Channel *chan, void *value)
{
    if (chan == NULL)
        return;

    pthread_mutex_lock(&chan->mu);

    queue_enqueue(chan->queue, value);

    pthread_cond_signal(&chan->cond);

    pthread_mutex_unlock(&chan->mu);
}

void *channel_recv(Channel *chan)
{
    if (chan == NULL)
        return NULL;

    pthread_mutex_lock(&chan->mu);

    while (queue_is_empty(chan->queue) && !chan->closed)
        pthread_cond_wait(&chan->cond, &chan->mu);

    if (chan->closed)
    {
        pthread_mutex_unlock(&chan->mu);
        return NULL;
    }

    void *value = queue_dequeue(chan->queue);

    pthread_mutex_unlock(&chan->mu);

    return value;
}

void channel_free(Channel *chan)
{
    if (chan == NULL)
        return;

    pthread_mutex_lock(&chan->mu);
    chan->closed = true;
    pthread_cond_broadcast(&chan->cond);
    pthread_mutex_unlock(&chan->mu);

    pthread_mutex_destroy(&chan->mu);
    pthread_cond_destroy(&chan->cond);
    queue_free(chan->queue);
    free(chan);
}