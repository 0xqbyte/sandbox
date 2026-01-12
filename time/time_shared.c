/*
Determines the amount of time necessary to run a command
from the command line. Program will report the amount of elapsed
time to run the specified command.
*/

#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>

#define SHM_NAME "/shm" /* shared memory object name */
#define SHM_MODE 0666   /* shared memory object mode */

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        fprintf(stderr, "Usage: time command\n");
        return 1;
    }

    char *cmd = argv[1];

    /*
        create the shared memory object
        file descriptor inherits to child process
    */
    int fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, SHM_MODE);
    if (fd < 0)
    {
        perror("shm_open");
        return 1;
    }

    /* size of shared memory object */
    const int shm_size = sizeof(struct timeval);

    /* configure the size of the shared memory object */
    if (ftruncate(fd, shm_size) != 0)
    {
        perror("ftruncate");
        return 1;
    }

    /* fork the current process */
    pid_t pid = fork();
    if (pid < 0)
    {
        perror("fork");
        return 1;
    }

    if (pid == 0) /* child process */
    {
        /* memory map the shared memory object */
        struct timeval *start_time = mmap(NULL, shm_size, PROT_WRITE, MAP_SHARED, fd, 0);
        if (start_time == MAP_FAILED)
        {
            perror("mmap");
            _exit(1);
        }

        /* record current ts to start_time */
        if (gettimeofday(start_time, NULL) != 0)
        {
            perror("gettimeofday");
            _exit(1);
        }

        /* execute specified command */
        int err = execvp(cmd, &argv[1]);
        if (err == -1)
            perror("execvp");

        /* unmap memory region */
        munmap(start_time, shm_size);

        /* close shared memory object */
        close(fd);

        _exit(1);
    }

    /* wait for the child process to complete */
    wait(NULL);

    /* memory map the shared memory object */
    struct timeval *start_time = mmap(NULL, shm_size, PROT_READ, MAP_SHARED, fd, 0);
    if (start_time == MAP_FAILED)
    {
        perror("mmap");
        return 1;
    }

    struct timeval end_time;
    struct timeval elapsed_time;

    /* record current ts to end_time */
    if (gettimeofday(&end_time, NULL) != 0)
    {
        perror("gettimeofday");
        return 1;
    }

    /* get difference between start_time and end_time */
    timersub(&end_time, start_time, &elapsed_time);

    printf("Elapsed time: %li.%li\n", elapsed_time.tv_sec, elapsed_time.tv_usec);

    /* remove shared memory object */
    shm_unlink(SHM_NAME);

    /* unmap memory region */
    munmap(start_time, shm_size);

    /* close shared memory object */
    close(fd);

    return 0;
}
