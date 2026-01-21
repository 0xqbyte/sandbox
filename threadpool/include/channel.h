#ifndef CHANNEL_H
#define CHANNEL_H

#include <pthread.h>
#include <stdbool.h>
#include "queue.h"

typedef struct
{
    pthread_mutex_t mu;
    pthread_cond_t cond;
    Queue *queue;
    bool closed;
} Channel;

Channel *channel_init();
void channel_send(Channel *ch, void *value);
void *channel_recv(Channel *ch);
void channel_close(Channel *ch);
void channel_free(Channel *ch);

#endif