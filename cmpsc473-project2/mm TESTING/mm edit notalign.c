/*
 * mm.c
 *
 * Name: Taylan Unal, Joseph Brauckmann
 * Description: 
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
// #define ALIGNMENT 16
#define ALIGNMENT (2 * sizeof(size_t))

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
    size_t isFree; // 0=not free, 1=free
} m_block;
/* End Structs */

// Defining Constants:
size_t BLOCK_SIZE = sizeof(m_block);
m_block* head; // initialize head/start block

/* rounds up to the nearest multiple of ALIGNMENT */
static size_t align(size_t x)
{
    return ALIGNMENT * ((x+ALIGNMENT-1)/ALIGNMENT);
}

typedef struct {
    size_t header;
    /*
     * We don't know what the size of the payload will be, so we will
     * declare it as a zero-length array.  This allow us to obtain a
     * pointer to the start of the payload.
     */
    uint8_t payload[];
} block_t;

static size_t round_up(size_t size, size_t n) {
    return (size + n - 1) / n * n;
}

static size_t get_size(block_t *block) {
    return block->header & ~0x1;
}

static void set_header(block_t *block, size_t size, bool is_allocated) {
    block->header = size | is_allocated;
}

/*
 * Initialize: returns false on error, true on success.
 */
bool mm_init(void)
{
    return true;
}

/*
 * Project2
 * Function: find_block
 * Description: Find next empty block
 * Input: An unsigned integer 'size' that defines the size of a memory block
 * Output: Returns applicable block for allocator to use
 */
m_block* find_block(size_t size) {
    m_block *temp_block = head->next; // hits exception here.
    while(temp_block != NULL){
        if(temp_block->isFree && temp_block->block_size >= size){ // verifies that the temp_block is isFree and has enough space to allocate
            return temp_block;
        }
        temp_block = temp_block->next; //advance to next temp_block if current fails
    }
    return NULL; //endcase if no free blocks found
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
    size = round_up(sizeof(block_t) + size, ALIGNMENT);
    block_t *block = mem_sbrk(size);
    if ((long) block < 0) {
        return NULL;
    }

    set_header(block, size, true);
    return block->payload;
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

    // Scans entire LinkedList, uses last_freed as temp variable.
    m_block* to_free = head;
    m_block* last_freed = head;

    while (to_free != NULL) {
        if(to_free->mem_ptr == ptr){
            to_free = ptr - to_free->block_size;
            to_free->isFree = 1;
            last_freed = to_free;
            break;
        }
        last_freed = to_free;
        to_free = to_free->next;
    }
    to_free->block_size = (last_freed->mem_ptr - to_free->mem_ptr);
    // Working up to 4th free in trace syn-array-short.rep.

    // Create block
    // POINTER PASSED IN IS END OF BLOCK, HAVE TO SUBTRACT THE BYTES OF SIZE OF BLOCK TO CONSIDER FREED
    // Makes sure that the current block's next block is not null and is free
    printf("to_free %p\n", (void *) to_free);
    printf("last_freed %p\n", (void *) last_freed);
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
        return malloc(align(size)); // if hits, will return garbled data
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
    /* Write code to check heap invariants here */
    /* IMPLEMENT THIS */
#endif /* DEBUG */
    return true;
}
