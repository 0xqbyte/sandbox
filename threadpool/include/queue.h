#ifndef QUEUE_H
#define QUEUE_H

#include <stddef.h>
#include <stdbool.h>

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
void queue_enqueue(Queue *q, void *value);
void *queue_dequeue(Queue *q);
bool queue_is_empty(Queue *q);
void queue_free(Queue *q);

#endif