#include <stdlib.h>
#include <stdbool.h>
#include "history_buffer.h"

HistoryBuffer *history_buffer_init(size_t cap)
{
    HistoryBuffer *buff = malloc(sizeof(HistoryBuffer));
    if (buff == NULL)
        return NULL;

    buff->head = NULL;
    buff->tail = NULL;
    buff->len = 0;
    buff->cap = cap;

    return buff;
}

void history_buffer_append(HistoryBuffer *buff, char *value)
{
    HistoryBufferNode *new_node = malloc(sizeof(HistoryBufferNode));
    if (new_node == NULL)
        return;

    new_node->value = value;

    if (buff->len >= buff->cap)
    {
        HistoryBufferNode *tmp = buff->head;
        buff->head = buff->head->next;
        free(tmp->value);
        free(tmp);
        buff->len--;
    }

    if (buff->head == NULL)
        buff->head = new_node;

    if (buff->tail != NULL)
        buff->tail->next = new_node;

    buff->tail = new_node;
    buff->len++;
}

char *history_buffer_last(HistoryBuffer *buff)
{
    if (buff == NULL)
        return NULL;

    if (buff->tail == NULL)
        return NULL;

    return buff->tail->value;
}

bool history_buffer_is_empty(HistoryBuffer *buff)
{
    if (buff == NULL)
        return true;

    return buff->len == 0;
}

void history_buffer_free(HistoryBuffer *buff)
{
    if (buff == NULL)
        return;

    HistoryBufferNode *ptr = buff->head;
    while (ptr != NULL)
    {
        HistoryBufferNode *tmp = ptr;
        ptr = ptr->next;
        free(tmp->value);
        free(tmp);
    }

    free(buff);
}
