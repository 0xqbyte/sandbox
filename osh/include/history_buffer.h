#ifndef HISTORY_BUFFER_H
#define HISTORY_BUFFER_H

#include <stdlib.h>
#include <stdbool.h>

typedef struct node
{
    char *value;
    struct node *next;
} HistoryBufferNode;

typedef struct
{
    HistoryBufferNode *head;
    HistoryBufferNode *tail;
    size_t len;
    size_t cap;
} HistoryBuffer;

HistoryBuffer *history_buffer_init(size_t cap);
void history_buffer_append(HistoryBuffer *buff, char *value);
char *history_buffer_last(HistoryBuffer *buff);
bool history_buffer_is_empty(HistoryBuffer *buff);
void history_buffer_free(HistoryBuffer *buff);

#endif