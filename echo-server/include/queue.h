#ifndef QUEUE_H
#define QUEUE_H

#include <stddef.h>

struct qnode
{
    void *value;
    struct qnode *next;
};

typedef struct
{
    struct qnode *head;
    struct qnode *tail;
    size_t len;
} Queue;

Queue *queue_init();
void queue_enqueue(Queue *queue, void *value);
void *queue_dequeue(Queue *queue);
bool queue_is_empty(Queue *queue);
void queue_free(Queue *queue);

#endif