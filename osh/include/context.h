#ifndef CONTEXT_H
#define CONTEXT_H

#include "history_buffer.h"

typedef struct
{
    char *line;
    char **args;
    int args_n;
    HistoryBuffer *h_buff;
    int output_fd;
    int input_fd;
} Context;

#endif