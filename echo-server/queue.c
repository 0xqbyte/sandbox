#include <stdlib.h>
#include <stdbool.h>
#include "queue.h"

Queue *queue_init()
{
    Queue *queue = (Queue *)malloc(sizeof(Queue));
    if (queue == NULL)
        return NULL;

    queue->head = NULL;
    queue->tail = NULL;
    queue->len = 0;

    return queue;
}

void queue_enqueue(Queue *queue, void *value)
{
    if (queue == NULL)
        return;

    struct qnode *node = (struct qnode *)malloc(sizeof(struct qnode));
    if (node == NULL)
        return;

    node->value = value;
    node->next = NULL;

    if (queue->tail != NULL)
        queue->tail->next = node;
    else
        queue->head = node;

    queue->tail = node;
    queue->len++;
}

void *queue_dequeue(Queue *queue)
{
    if (queue == NULL || queue->head == NULL)
        return NULL;

    struct qnode *first = queue->head;
    void *value = first->value;

    queue->head = queue->head->next;
    if (queue->head == NULL)
        queue->tail = NULL;

    queue->len--;
    free(first);
    return value;
}

bool queue_is_empty(Queue *queue)
{
    if (queue == NULL)
        return true;

    return queue->len == 0;
}

void queue_free(Queue *queue)
{
    if (queue == NULL)
        return;

    while (queue->head != NULL)
    {
        struct qnode *tmp = queue->head;
        queue->head = tmp->next;
        free(tmp);
    }

    free(queue);
}