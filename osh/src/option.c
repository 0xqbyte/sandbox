#include <string.h>
#include <fcntl.h>
#include <stdio.h>
#include "option.h"

bool has_opt_concurent(Context *ctx)
{
    if (strcmp(ctx->args[ctx->args_n - 1], OPT_CONCURRENT) == 0)
    {
        ctx->args[--ctx->args_n] = NULL;
        return true;
    }
    return false;
}

int handle_opt_output_redirect(Context *ctx)
{
    if (ctx->args_n < 3)
        return 0;

    if (strcmp(ctx->args[ctx->args_n - 2], OPT_OUTPUT_REDIRECT) != 0)
        return 0;

    char *filename = ctx->args[ctx->args_n - 1];

    ctx->output_fd = open(filename, O_CREAT | O_WRONLY | O_TRUNC, 0644);
    if (ctx->output_fd < 0)
        return ctx->output_fd;

    ctx->args[ctx->args_n - 1] = NULL;
    ctx->args[ctx->args_n - 2] = NULL;

    ctx->args_n -= 2;

    return 0;
}

int handle_opt_input_redirect(Context *ctx)
{
    if (ctx->args_n < 3)
        return 0;

    if (strcmp(ctx->args[ctx->args_n - 2], OPT_INPUT_REDIRECT) != 0)
        return 0;

    char *filename = ctx->args[ctx->args_n - 1];
    ctx->input_fd = open(filename, O_RDONLY);
    if (ctx->input_fd < 0)
        return ctx->input_fd;

    ctx->args[ctx->args_n - 1] = NULL;
    ctx->args[ctx->args_n - 2] = NULL;

    ctx->args_n -= 2;

    return 0;
}

int has_opt_pipe(Context *ctx)
{
    if (ctx->args_n < 3)
        return -1;

    for (int i = 0; i < ctx->args_n; i++)
        if (strcmp(ctx->args[i], OPT_PIPE) == 0)
            return i;

    return -1;
}