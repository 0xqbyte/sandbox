#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include "history_buffer.h"
#include "context.h"
#include "command.h"
#include "option.h"

#define PROMPT "osh> "
#define MAX_LINE 80         /* The maximum length command */
#define HISTORY_BUFF_CAP 50 /* The maximum entries in history */

static void reap(int sig);
static void process(Context *ctx);
static int read_line(char *line);
static int fill_args(char *line, char **args);

static void fatal(const char *message)
{
    perror(message);
    exit(EXIT_FAILURE);
}

int main()
{
    bool should_run = true; /* flag to determine when to exit program */
    signal(SIGCHLD, reap);

    HistoryBuffer *h_buff = history_buffer_init(HISTORY_BUFF_CAP);
    if (h_buff == NULL)
        fatal("history_buffer_init");

    while (should_run)
    {
        printf(PROMPT);
        fflush(stdout);

        bool concurrent = false;            /* child process runs in background */
        char *args[MAX_LINE / 2 + 1] = {0}; /* command line arguments */

        Context ctx = {
            .h_buff = h_buff,
            .output_fd = STDOUT_FILENO,
            .input_fd = STDIN_FILENO,
        };

        char line[MAX_LINE];
        int n = read_line(line);
        if (n <= 0)
        {
            if (n == 0)
                continue;

            if (n == EOF)
            {
                should_run = false;
                break;
            }
        }

        ctx.line = line;

        /* check line for builtin commands */

        if (is_cmd_exit(&ctx))
        {
            should_run = false;
            break;
        }

        if (is_cmd_history(&ctx))
        {
            cmd_history(&ctx);
            continue;
        }

        if (is_cmd_last(&ctx) && cmd_last(&ctx))
            continue;

        if (!is_cmd_last(&ctx) && append_history(&ctx))
            fatal("append_history");

        /* tmp: pass copy of the line to fill_args */
        char input[strlen(line) + 1];
        strcpy(input, line);
        input[strlen(line)] = '\0';

        ctx.args_n = fill_args(input, args);
        ctx.args = args;

        /* check args for options */
        if (has_opt_concurent(&ctx))
            concurrent = true;

        if (handle_opt_output_redirect(&ctx))
            fatal("handle_opt_output_redirect");

        if (handle_opt_input_redirect(&ctx))
            fatal("handle_opt_input_redirect");

        int pipe_idx = has_opt_pipe(&ctx);
        if (pipe_idx != -1)
        {
            char *producer_args[pipe_idx + 1];
            for (int i = 0; i < pipe_idx; i++)
                producer_args[i] = ctx.args[i];
            producer_args[pipe_idx] = NULL;

            char **consumer_args = &ctx.args[pipe_idx + 1];

            int pipefd[2];

            /* pipefd[0] - read end */
            /* pipefd[1] - write end */
            if (pipe(pipefd) != 0)
                fatal("pipe");

            pid_t producer_pid = fork();
            if (producer_pid < 0)
                fatal("fork");

            if (producer_pid == 0) /* launch producer process */
            {
                if (dup2(pipefd[1], STDOUT_FILENO) < 0)
                {
                    perror("dup2");
                    exit(1);
                }

                close(pipefd[0]);
                close(pipefd[1]);

                int err = execvp(producer_args[0], producer_args);
                if (err < 0)
                {
                    perror("execvp");
                    exit(1);
                }

                exit(EXIT_SUCCESS);
            }

            pid_t consumer_pid = fork();
            if (consumer_pid < 0)
                fatal("fork");

            if (consumer_pid == 0) /* launch consumer process */
            {
                close(pipefd[1]);

                if (dup2(pipefd[0], STDIN_FILENO) < 0)
                {
                    perror("dup2");
                    exit(1);
                }

                close(pipefd[0]);
                close(pipefd[1]);

                int err = execvp(consumer_args[0], consumer_args);
                if (err < 0)
                {
                    perror("execvp");
                    exit(1);
                }

                exit(EXIT_SUCCESS);
            }

            close(pipefd[0]);
            close(pipefd[1]);

            waitpid(producer_pid, NULL, 0);
            waitpid(consumer_pid, NULL, 0);

            continue;
        }

        pid_t pid = fork();
        if (pid < 0)
            fatal("fork");

        /* child process */
        if (pid == 0)
            process(&ctx);

        if (!concurrent)
            waitpid(pid, NULL, 0);

        if (ctx.output_fd != STDOUT_FILENO)
            close(ctx.output_fd);

        if (ctx.input_fd != STDIN_FILENO)
            close(ctx.input_fd);
    }

    history_buffer_free(h_buff);

    return 0;
}

static void process(Context *ctx)
{
    if (dup2(ctx->output_fd, STDOUT_FILENO) < 0)
    {
        perror("dup2");
        exit(1);
    }

    if (dup2(ctx->input_fd, STDIN_FILENO) < 0)
    {
        perror("dup2");
        exit(1);
    }

    if (ctx->output_fd != STDOUT_FILENO)
        close(ctx->output_fd);

    if (ctx->input_fd != STDIN_FILENO)
        close(ctx->input_fd);

    int err = execvp(ctx->args[0], ctx->args);
    if (err < 0)
    {
        perror("execvp");
        exit(1);
    }

    exit(EXIT_SUCCESS);
}

static int read_line(char *line)
{
    int c, n = 0;
    while (n < MAX_LINE - 1 && (c = fgetc(stdin)) != '\n')
    {
        if (c == EOF)
            return EOF;

        *line++ = c;
        n++;
    }
    *line = '\0';
    return n;
}

/** todo:
 * handle multiple spaces
 * handle tabs
 * handle quotes
 */
static int fill_args(char *line, char **args)
{
    int n = 0;
    const char *delim = " ";

    char *arg = strtok(line, delim);
    while (arg != NULL)
    {
        *args++ = arg;
        arg = strtok(NULL, delim);
        n++;
    }

    *args = NULL;

    return n;
}

static void reap(int sig)
{
    (void)sig;
    while (waitpid(-1, NULL, WNOHANG) > 0)
        ;
}
