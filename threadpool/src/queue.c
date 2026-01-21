#include <stdlib.h>
#include <stdbool.h>
#include "queue.h"

Queue *queue_init()
{
    Queue *q = malloc(sizeof(Queue));
    if (q == NULL)
        return NULL;

    q->head = NULL;
    q->tail = NULL;
    q->len = 0;

    return q;
}

void queue_enqueue(Queue *q, void *value)
{
    if (q == NULL)
        return;

    struct qnode *node = malloc(sizeof(struct qnode));
    if (node == NULL)
        return;

    node->value = value;
    node->next = NULL;

    if (q->tail != NULL)
        q->tail->next = node;
    else
        q->head = node;

    q->tail = node;
    q->len++;
}

void *queue_dequeue(Queue *q)
{
    if (q == NULL || q->head == NULL)
        return NULL;

    struct qnode *head = q->head;
    void *value = head->value;

    q->head = head->next;
    if (q->head == NULL)
        q->tail = NULL;

    q->len--;
    free(head);
    return value;
}

bool queue_is_empty(Queue *q)
{
    return q == NULL || q->len == 0;
}

void queue_free(Queue *q)
{
    if (q == NULL)
        return;

    while (q->head != NULL)
    {
        struct qnode *tmp = q->head;
        q->head = tmp->next;
        free(tmp);
    }

    free(q);
}