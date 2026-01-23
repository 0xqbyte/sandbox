#ifndef MEM_BLOCK_H
#define MEM_BLOCK_H

#include <stddef.h>

enum BlockState
{
    FREE,
    RESERVED
};

enum CandidateSearchingApproach
{
    F, /* first-fit */
    B, /* best-fit */
    W  /* worst-fit */
};

/* single memory block */
typedef struct memory_block
{
    size_t size;           /* size of the block in bytes */
    int start;             /* start address of the block */
    int end;               /* last address of the block */
    enum BlockState state; /* current state of the block */
    const char *process;   /* owner process of the reserved block */

    struct memory_block *next; /* link to the next block */
    struct memory_block *prev; /* link to the previous block */
} MemoryBlock;

/* sequence of the contigious blocks */
typedef struct
{
    size_t size;         /* total size of available space in bytes */
    MemoryBlock *blocks; /* pointer to the list of blocks */
    int n;               /* current number of blocks */
} Memory;

/* initialize memory */
Memory *memory_init(size_t size);

/* requests memory for process */
/* 0 - success */
/* 1 - error */
int memory_request(Memory *mem, const char *process, size_t size, enum CandidateSearchingApproach approach);
void memory_release(Memory *mem, const char *process);
void memory_compact(Memory *mem);
void memory_stat(Memory *mem);
void memory_free(Memory *mem);

#endif