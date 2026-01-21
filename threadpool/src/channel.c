#include <stdlib.h>
#include "channel.h"
#include "queue.h"

Channel *channel_init()
{
    Channel *ch = malloc(sizeof(Channel));
    if (ch == NULL)
        return NULL;

    ch->queue = queue_init();
    if (ch->queue == NULL)
    {
        free(ch);
        return NULL;
    }

    if (pthread_mutex_init(&ch->mu, NULL) != 0)
    {
        queue_free(ch->queue);
        free(ch);
        return NULL;
    }

    if (pthread_cond_init(&ch->cond, NULL) != 0)
    {
        pthread_mutex_destroy(&ch->mu);
        queue_free(ch->queue);
        free(ch);
        return NULL;
    }

    ch->closed = false;

    return ch;
}

void channel_send(Channel *ch, void *value)
{
    if (ch == NULL)
        return;

    pthread_mutex_lock(&ch->mu);

    queue_enqueue(ch->queue, value);

    pthread_cond_signal(&ch->cond);

    pthread_mutex_unlock(&ch->mu);
}

void *channel_recv(Channel *ch)
{
    if (ch == NULL)
        return NULL;

    pthread_mutex_lock(&ch->mu);

    while (queue_is_empty(ch->queue) && !ch->closed)
        pthread_cond_wait(&ch->cond, &ch->mu);

    /* drain queue first */
    if (ch->closed && queue_is_empty(ch->queue))
    {
        pthread_mutex_unlock(&ch->mu);
        return NULL;
    }

    void *value = queue_dequeue(ch->queue);

    pthread_mutex_unlock(&ch->mu);

    return value;
}

void channel_close(Channel *ch)
{
    if (ch == NULL)
        return;

    pthread_mutex_lock(&ch->mu);
    ch->closed = true;
    pthread_cond_broadcast(&ch->cond);
    pthread_mutex_unlock(&ch->mu);
}

void channel_free(Channel *ch)
{
    if (ch == NULL)
        return;

    pthread_mutex_destroy(&ch->mu);
    pthread_cond_destroy(&ch->cond);
    queue_free(ch->queue);
    free(ch);
}