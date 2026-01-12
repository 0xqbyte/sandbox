#include <stdio.h>
#include <string.h>
#include "command.h"
#include "context.h"

#define HISTORY_EMPTY_MESSAGE "No commands in history.\n"

bool is_cmd_exit(Context *ctx)
{
    return strcmp(ctx->line, CMD_EXIT) == 0;
}

bool is_cmd_history(Context *ctx)
{
    return strcmp(ctx->line, CMD_HISTORY) == 0;
}

bool is_cmd_last(Context *ctx)
{
    return strcmp(ctx->line, CMD_LAST) == 0;
}

void cmd_history(Context *ctx)
{
    int i = 1;
    HistoryBufferNode *head = ctx->h_buff->head;

    while (head != NULL)
    {
        printf("%4d  %s\n", i++, head->value);
        head = head->next;
    }
}

bool cmd_last(Context *ctx)
{
    if (history_buffer_is_empty(ctx->h_buff))
    {
        printf(HISTORY_EMPTY_MESSAGE);
        return true;
    }

    strcpy(ctx->line, history_buffer_last(ctx->h_buff));
    printf("%s\n", ctx->line);
    return false;
}

int append_history(Context *ctx)
{    
    char *history_line = calloc(strlen(ctx->line) + 1, sizeof(char));
    if (history_line == NULL)
        return 1;
    
    strcpy(history_line, ctx->line);

    if (history_buffer_is_empty(ctx->h_buff))
    {
        history_buffer_append(ctx->h_buff, history_line);
        return 0;
    }

    /* check with the last command to avoid duplication */
    char *last = history_buffer_last(ctx->h_buff);
    if (strcmp(history_line, last) == 0)
    {
        free(history_line);
        return 0;
    }

    history_buffer_append(ctx->h_buff, history_line);

    return 0;
}