#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cma_memory.h"

static void memory_block_stat(MemoryBlock *block);
static MemoryBlock *get_candidate_block(Memory *mem, size_t size, enum CandidateSearchingApproach approach);
static MemoryBlock *first_fit_block(Memory *mem, size_t size);
static MemoryBlock *best_fit_block(Memory *mem, size_t size);
static MemoryBlock *worst_fit_block(Memory *mem, size_t size);

Memory *memory_init(size_t size)
{
    Memory *mem = malloc(sizeof(Memory));
    if (!mem)
        return NULL;

    mem->blocks = malloc(sizeof(MemoryBlock));
    if (!mem->blocks)
    {
        free(mem);
        return NULL;
    }

    mem->blocks->size = size;
    mem->blocks->start = 0;
    mem->blocks->end = size - 1;
    mem->blocks->state = FREE;
    mem->blocks->process = NULL;

    mem->size = size;
    mem->n = 1;

    return mem;
}

int memory_request(Memory *mem, const char *process, size_t size, enum CandidateSearchingApproach approach)
{
    /* first-fit approach allocation */
    /* todo: best-fit */
    MemoryBlock *candidate_block = get_candidate_block(mem, size, approach);
    if (candidate_block == NULL)
        return 1;

    /**
     * if candidate block size is equal to requested size
     * allocate entire candidate block
     */
    if (candidate_block->size == size)
    {
        candidate_block->state = RESERVED;
        candidate_block->process = process;
        return 0;
    }

    /**
     * if request size is less than block size
     * split candidate block into two blocks
     */
    MemoryBlock *reserved_block = malloc(sizeof(MemoryBlock));
    if (!reserved_block)
        return 1;

    reserved_block->size = size;
    reserved_block->start = candidate_block->start;
    reserved_block->end = reserved_block->start + size - 1;
    reserved_block->state = RESERVED;
    reserved_block->process = process;
    reserved_block->next = candidate_block;
    reserved_block->prev = candidate_block->prev;

    if (candidate_block->prev != NULL)
        candidate_block->prev->next = reserved_block;
    else
        mem->blocks = reserved_block;

    candidate_block->size -= size;
    candidate_block->start = reserved_block->end + 1;
    candidate_block->prev = reserved_block;

    mem->n++;

    return 0;
}

void memory_release(Memory *mem, const char *process)
{
    MemoryBlock *candidate = NULL;
    MemoryBlock *block = mem->blocks;
    while (block != NULL)
    {
        if (block->state != FREE && strcmp(block->process, process) == 0)
        {
            candidate = block;
            break;
        }
        block = block->next;
    }

    if (candidate == NULL)
        return;

    candidate->state = FREE;
    free((char *)candidate->process);
    candidate->process = NULL;

    /* check adjacent memory blocks */
    MemoryBlock *prev_block = candidate->prev;
    MemoryBlock *next_block = candidate->next;

    if (prev_block != NULL && prev_block->state == FREE)
    {
        candidate->size += prev_block->size;
        candidate->start = prev_block->start;
        candidate->prev = prev_block->prev;

        if (prev_block->prev != NULL)
            prev_block->prev->next = candidate;
        else
            mem->blocks = candidate;

        free(prev_block);
        mem->n--;
    }

    if (next_block != NULL && next_block->state == FREE)
    {
        candidate->size += next_block->size;
        candidate->end = next_block->end;
        candidate->next = next_block->next;

        if (next_block->next != NULL)
            next_block->next->prev = candidate;

        free(next_block);
        mem->n--;
    }
}

/* todo: check block addresses after compact */
void memory_compact(Memory *mem)
{
    if (mem->n < 3)
        return;

    MemoryBlock *free_block = malloc(sizeof(MemoryBlock));
    if (free_block == NULL)
        return;

    free_block->size = 0;
    free_block->start = 0;
    free_block->end = 0;
    free_block->state = FREE;
    free_block->process = NULL;
    free_block->next = NULL;
    free_block->prev = NULL;

    int current = 0;
    MemoryBlock *tmp = NULL;
    MemoryBlock *last = NULL;
    MemoryBlock *block = mem->blocks;
    while (block != NULL)
    {
        if (block->state == FREE)
        {
            free_block->size += block->size;

            if (block->prev != NULL)
                block->prev->next = block->next;

            if (block->next != NULL)
                block->next->prev = block->prev;

            tmp = block;
            block = block->next;
            free(tmp);
            continue;
        }

        if (block->state == RESERVED)
        {
            block->start = current;
            block->end = current + block->size - 1;
            current = block->end + 1;
        }

        last = block;
        block = block->next;
    }

    /* if all blocks are reserved */
    if (free_block->size == 0)
    {
        free(free_block);
        return;
    }

    free_block->start = last->end + 1;
    free_block->end = free_block->start + free_block->size - 1;
    free_block->prev = last;
    last->next = free_block;
    if (last->prev == NULL)
        mem->blocks = last;
}

void memory_stat(Memory *mem)
{
    MemoryBlock *block = mem->blocks;
    while (block != NULL)
    {
        memory_block_stat(block);
        block = block->next;
    }
}

void memory_free(Memory *mem)
{
    while (mem->blocks != NULL)
    {
        MemoryBlock *block = mem->blocks;
        mem->blocks = mem->blocks->next;
        free(block);
    }

    free(mem);
}

static void memory_block_stat(MemoryBlock *block)
{
    if (block->state == FREE)
        printf("[%d:%d] FREE size=%ld\n", block->start, block->end, block->size);
    else if (block->state == RESERVED)
        printf("[%d:%d] %s size=%ld\n", block->start, block->end, block->process, block->size);
    else
        printf("[%d:%d] invalid state \n", block->start, block->end);
}

static MemoryBlock *get_candidate_block(
    Memory *mem,
    size_t size,
    enum CandidateSearchingApproach approach)
{
    switch (approach)
    {
    case F:
        return first_fit_block(mem, size);
    case B:
        return best_fit_block(mem, size);
    case W:
        return worst_fit_block(mem, size);
    default:
        return first_fit_block(mem, size);
    }
}

/**
 * Searches the first free memory block that is big enough.
 */
static MemoryBlock *first_fit_block(Memory *mem, size_t size)
{
    MemoryBlock *block = mem->blocks;
    while (block != NULL)
    {
        if (block->state == FREE && block->size >= size)
            return block;
        block = block->next;
    }

    return NULL;
}

/**
 * Searches the smallest free memory block that is big enough.
 */
static MemoryBlock *best_fit_block(Memory *mem, size_t size)
{
    size_t best_fit_size = 0;
    MemoryBlock *candidate = NULL;
    MemoryBlock *block = mem->blocks;
    while (block != NULL)
    {
        if (block->state == FREE && block->size >= size)
        {
            if (best_fit_size == 0)
                best_fit_size = block->size;

            if (block->size <= best_fit_size)
            {
                candidate = block;
                best_fit_size = block->size;
            }
        }
        block = block->next;
    }

    return candidate;
}

/**
 * Searches the largest free memory block.
 */
static MemoryBlock *worst_fit_block(Memory *mem, size_t size)
{
    size_t largest_size = 0;
    MemoryBlock *candidate = NULL;
    MemoryBlock *block = mem->blocks;
    while (block != NULL)
    {
        if (block->state == FREE && block->size >= size)
        {
            if (block->size >= largest_size)
            {
                candidate = block;
                largest_size = block->size;
            }
        }
        block = block->next;
    }

    return candidate;
}