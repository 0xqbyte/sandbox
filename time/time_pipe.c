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

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        fprintf(stderr, "Usage: time command\n");
        return 1;
    }

    char *cmd = argv[1];

    int pipefd[2];

    /* pipefd[0] - read end */
    /* pipefd[1] - write end */

    if (pipe(pipefd) != 0)
    {
        perror("pipe");
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
        /* close the unused read end of the pipe */
        close(pipefd[0]);

        struct timeval start_time;

        /* record current ts to start_time */
        if (gettimeofday(&start_time, NULL) != 0)
        {
            perror("gettimeofday");
            _exit(1);
        }

        /* write start_time to pipe */
        ssize_t n = write(pipefd[1], &start_time, sizeof(start_time));
        if (n < 0)
        {
            perror("write");
            _exit(1);
        }

        /* close write end of the pipe */
        close(pipefd[1]);

        /* execute specified command */
        int err = execvp(cmd, &argv[1]);
        if (err == -1)
            perror("execvp");

        _exit(1);
    }

    /* close the unused write end of the pipe */
    close(pipefd[1]);

    struct timeval start_time;
    struct timeval end_time;
    struct timeval elapsed_time;

    /* read start_time from pipe */
    ssize_t n = read(pipefd[0], &start_time, sizeof(start_time));
    if (n < 0)
    {
        perror("read");
        return 1;
    }

    /* close read end of the pipe */
    close(pipefd[0]);

    /* wait for the child process to complete */
    wait(NULL);

    /* record current ts to end_time */
    if (gettimeofday(&end_time, NULL) != 0)
    {
        perror("gettimeofday");
        return 1;
    }

    /* get difference between start_time and end_time */
    timersub(&end_time, &start_time, &elapsed_time);

    printf("Elapsed time: %li.%li\n", elapsed_time.tv_sec, elapsed_time.tv_usec);

    return 0;
}