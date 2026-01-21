#include <stdio.h>
#include <unistd.h>
#include "pool.h"

static void task(void *p)
{
    char *filename = p;
    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        perror("fopen");
        return;
    }

    int bytes = 0;
    int c;
    while ((c = fgetc(file)) != EOF)
        bytes++;

    fclose(file);

    printf("%s: %d bytes\n", filename, bytes);
}

int main()
{
    Pool *pool = pool_init(7);
    if (!pool)
    {
        perror("pool_init");
        return 1;
    }

    char *filenames[] = {
        "/etc/passwd",
        "/etc/group",
        "./src/channel.c",
        "./src/main.c",
        "./src/pool.c",
        "./src/queue.c",
        "./src/wait_group.c",
    };

    for (int i = 0; i < 7; ++i)
        pool_submit(pool, task, filenames[i]);

    pool_shutdown(pool);

    return 0;
}

