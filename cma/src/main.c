#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "cma_memory.h"

#define PROMPT "allocator> "
#define MAX_LINE 80  /* The maximum length command */
#define INIT_SIZE 10 /* initial size of memory in bytes */
#define APPROACH W

#define CMD_REQUEST "RQ"
#define CMD_RELEASE "RL"
#define CMD_COMPACT "C"
#define CMD_STAT "STAT"
#define CMD_EXIT "X"

#define MSG_INVALID_COMMAND "invalid command"

static int read_line(char *line);
static int fill_args(char *line, char **args);
char *strdup(const char *src);

int main()
{
    Memory *mem = memory_init(INIT_SIZE);

    char line[MAX_LINE];
    char *args[MAX_LINE / 2 + 1] = {0};
    int n, args_n;

    while (1)
    {
        printf(PROMPT);

        n = read_line(line);
        if (n == EOF)
            break;

        if (n == 0)
            continue;

        char *input = strdup(line);
        args_n = fill_args(input, args);
        (void)args_n;

        char *cmd = args[0];

        if (strcmp(cmd, CMD_REQUEST) == 0)
        {
            if (args_n != 4)
            {
                printf(MSG_INVALID_COMMAND "\n");
                continue;
            }

            char *process = strdup(args[1]);
            size_t size = atoi(args[2]);
            char *approach = args[3];

            enum CandidateSearchingApproach s_approach = F;
            if (strcmp(approach, "W") == 0)
                s_approach = W;

            if (strcmp(approach, "B") == 0)
                s_approach = B;

            if (memory_request(mem, process, size, s_approach) != 0)
                printf("error\n");
        }

        if (strcmp(cmd, CMD_RELEASE) == 0)
        {
            if (args_n != 2)
            {
                printf(MSG_INVALID_COMMAND "\n");
                continue;
            }

            char *process = args[1];
            memory_release(mem, process);
        }

        if (strcmp(cmd, CMD_COMPACT) == 0)
            memory_compact(mem);

        if (strcmp(cmd, CMD_STAT) == 0)
            memory_stat(mem);

        if (strcmp(cmd, CMD_EXIT) == 0)
            break;
    }

    memory_free(mem);

    return 0;
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

char *strdup(const char *src)
{
    char *dst = malloc(strlen(src) + 1);
    if (dst == NULL)
        return NULL;
    strcpy(dst, src);
    return dst;
}