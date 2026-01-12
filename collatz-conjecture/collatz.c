#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>

#define USAGE "Usage: collatz number\n"
#define SHM_NAME "/shm"            /* shared memory object name */
#define SHM_MODE 0666              /* shared memory object mode */
#define SHM_SIZE sizeof(int) * 100 /* shared memory object size */

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        fprintf(stderr, USAGE);
        return 1;
    }

    int n = atoi(argv[1]);
    if (n == 0)
    {
        fprintf(stderr, USAGE);
        return 1;
    }

    int fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, SHM_MODE);
    if (fd < 0)
    {
        perror("shm_open");
        return 1;
    }

    if (ftruncate(fd, SHM_SIZE) != 0)
    {
        perror("ftruncate");
        return 1;
    }

    pid_t pid = fork();
    if (pid < 0)
    {
        perror("fork");
        return 1;
    }

    if (pid == 0)
    {
        int *buff = mmap(NULL, SHM_SIZE, PROT_WRITE, MAP_SHARED, fd, 0);
        if (buff == MAP_FAILED)
        {
            perror("mmap");
            _exit(1);
        }

        int *p = buff;

        while (n > 1)
        {
            *p++ = n;

            if (n % 2 == 0)
                n = n / 2;
            else
                n = 3 * n + 1;
        }

        *p++ = n;

        *p = -1; /* last number as terminator */

        munmap(buff, SHM_SIZE);

        close(fd);

        _exit(0);
    }

    wait(NULL);

    int *buff = mmap(NULL, SHM_SIZE, PROT_READ, MAP_SHARED, fd, 0);
    if (buff == MAP_FAILED)
    {
        perror("mmap");
        return 1;
    }

    int *p = buff;
    while (*p != -1)
        printf("%d ", *p++);
    printf("\n");

    shm_unlink(SHM_NAME);

    munmap(buff, SHM_SIZE);

    close(fd);

    return 0;
}