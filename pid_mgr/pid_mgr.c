#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#define MIN_PID 300
#define MAX_PID 5000

/* total number of possible pids */
const int num_pids = MAX_PID - MIN_PID + 1;

/* number of extra bits */
const int extra_bits = num_pids % CHAR_BIT;

/* number of bytes with remaing extra byte */
const int n_bytes = (num_pids / CHAR_BIT) + (extra_bits > 0 ? 1 : 0);

int allocate_map(void);
int allocate_pid();
void releaase_pid(int pid);
int get(int i);
void allocate_bit(int i);
void release_bit(int i);

unsigned char *map;

int main()
{
    int err = allocate_map();
    if (err != 1)
    {
        perror("allocate_map");
        return 1;
    }

    for (int i = 0; i < 17; ++i)
    {
        int pid = allocate_pid();
        if (pid < 0)
        {
            printf("no avaialble pids\n");
            continue;
        }
        printf("pid=%d\n", pid);
    }

    free(map);

    return 0;
}

/*
    allocate n_bytes area where each bit
    represents the availability of i'th pid,
    value 0 at i'th bit indicates that pid is available
    value 1 indicates that the pid is currently in use
*/
int allocate_map(void)
{
    map = malloc(n_bytes);
    if (map == NULL)
        return -1;

    for (int i = 0; i < n_bytes; i++)
        map[i] = 0;

    return 1;
}

/*
    retrieves the value of i'th bit from map
    -1 - error
    1  - bit is currently in use
    0  - bit is available
*/
int get(int i)
{
    if (i < 0 || i >= num_pids)
        return -1;

    int byte = i / CHAR_BIT;
    int bit = i % CHAR_BIT;
    return map[byte] & (1 << (CHAR_BIT - 1 - bit)) ? 1 : 0;
}

int allocate_pid()
{
    for (int i = 0; i < num_pids; i++)
    {
        int bit = get(i);
        if (bit == 0)
        {
            allocate_bit(i);
            return MIN_PID + i;
        }
    }

    return -1;
}

void releaase_pid(int pid)
{
    if (pid < MIN_PID || pid > MAX_PID)
        return;

    release_bit(pid - MIN_PID);
}

void allocate_bit(int i)
{
    int byte = i / CHAR_BIT;
    int bit = i % CHAR_BIT;
    map[byte] |= (1 << (CHAR_BIT - 1 - bit));
}

void release_bit(int i)
{
    int byte = i / CHAR_BIT;
    int bit = i % CHAR_BIT;
    map[byte] &= ~(1 << (CHAR_BIT - 1 - bit));
}