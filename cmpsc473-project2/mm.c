/*
 * mm.c
 *
 * Name: Taylan Unal, Joseph Brauckmann
 * Description:
 * mm.c implements malloc, free, realloc, calloc and their helper functions to provide the system a barebones solution 
 * to allocating, freeing, and reallocating memory in the heap.
 * 
 * Results:
 * Average utilization = 28.9%. Average throughput = 209627 Kops/sec
 * Score: Checkpoint 1: 50 / 50, Checkpoint 2: 40 / 100, Final: 40 / 100
 *
 * NOTE TO STUDENTS: Replace this header comment with your own header
 * comment that gives a high level description of your solution.
 * Also, read malloclab.pdf carefully and in its entirety before beginning.
 *
 */
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdint.h>

#include "mm.h"
#include "memlib.h"

/*
 * If you want to enable your debugging output and heap checker code,
 * uncomment the following line. Be sure not to have debugging enabled
 * in your final submission.
 */
#define DEBUG

#ifdef DEBUG
/* When debugging is enabled, the underlying functions get called */
#define dbg_printf(...) printf(__VA_ARGS__)
#define dbg_assert(...) assert(__VA_ARGS__)
#else
/* When debugging is disabled, no code gets generated */
#define dbg_printf(...)
#define dbg_assert(...)
#endif /* DEBUG */

/* do not change the following! */
#ifdef DRIVER
/* create aliases for driver tests */
#define malloc mm_malloc
#define free mm_free
#define realloc mm_realloc
#define calloc mm_calloc
#define memset mem_memset
#define memcpy mem_memcpy
#endif /* DRIVER */

/* What is the correct alignment? */
#define ALIGNMENT 16
// NOTE: Increasing alignment reduces utilization.

/* Begin Structs */
typedef struct heap_info{
    struct heap_info *prev;
    struct heap_info *next;

    size_t heap_total_size;
    size_t heap_free_size;
    size_t total_blocks;
} m_heap;

typedef struct block_info{
    struct block_info* prev;
    struct block_info* next;
    void* mem_ptr; // denotes where the header of the block is
    size_t block_size; // tracks the size of the block
    size_t isFree; // 0=free, 1=allocated
} m_block;

typedef struct free_list{
    struct list_info* prev;
    struct list_info* next;
    void* free_ptr;
    size_t free_size;
}f_list;
/* End Structs */

// Defining Constants:
size_t BLOCK_SIZE = sizeof(m_block);
m_block* head; // initialize head/start block
f_list* free_list; // initialize free list

/* rounds up to the nearest multiple of ALIGNMENT */
static size_t align(size_t x)
{
    return ALIGNMENT * ((x+ALIGNMENT-1)/ALIGNMENT);
}

/*
 * Initialize: returns false on error, true on success.
 */
bool mm_init(void) //was bool mm_init(void)
{
    return true;
}

/*
 * Project2
 * Function: malloc
 * Description: Allocates a block of memory, returns a pointer to the block
 * Input: An unsigned integer 'size' that defines the size of the memory block to allocate
 * Output: A pointer to the memory block that was allocated
 */
void* malloc(size_t size)
{
    if(size == 0){
        return NULL;
    } else{
        void *ptr = mem_sbrk(align(size)); //note, increasing sbrk size reduces utilziation ratio
        if(!ptr){ // indicates mem_sbrk failed
            return NULL;
        } else{ //endcase, ensures pointer errors caught
            return ptr;
        }
    }
}

/*
 * Project2
 * Function: free
 * Description: Deallocates a block of memory that was previously allocated by malloc/calloc/realloc
 * Input: A pointer to a memory block that was allocated
 * Output: None
 */
void free(void* ptr)
{
    if(ptr == NULL){
        return;
    }
}

/*
 * Project2
 * Function: realloc
 * Description: Resizes memory block at address from pointer that was allocated by malloc/calloc
 * Input: A pointer to a previously allocated memory block, a size value in bytes
 * Output: None
 */
void* realloc(void* oldptr, size_t size)
{
    // NOTE: utilize malloc in realloc
    void *newptr = malloc(size); //initialize new pointer as temporary

    if(size <= 0){
        free(oldptr);
        return NULL;
    }

    if(oldptr == 0){
        return malloc(size);
    }

    if(newptr != 0){
        memcpy(newptr, oldptr, size); //move 'size' bytes of data from old pointer to new pointer
        free(oldptr);
    }
    return newptr;
}

/*
 * calloc
 * This function is not tested by mdriver, and has been implemented for you.
 */
void* calloc(size_t nmemb, size_t size)
{
    void* ptr;
    size *= nmemb;
    ptr = malloc(size);
    if (ptr != 0) {
        memset(ptr, 0, size);
    }
    return ptr;
}

/*
 * Returns whether the pointer is in the heap.
 * May be useful for debugging.
 */
static bool in_heap(const void* p)
{
    return p <= mem_heap_hi() && p >= mem_heap_lo();
}

/*
 * Returns whether the pointer is aligned.
 * May be useful for debugging.
 */
static bool aligned(const void* p)
{
    size_t ip = (size_t) p;
    return align(ip) == ip;
}

/*
 * mm_checkheap
 */
bool mm_checkheap(int lineno)
{
#ifdef DEBUG
#endif /* DEBUG */
    return true;
}