/*
 * mm.c
 * MODIFYING WORKING VERSION FROM COMMIT: bfd6c82 on 3/12/21

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
// #define DEBUG

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
// #define ALIGNMENT (2 * sizeof(size_t))

// struct block_info{
//     struct block_info *prev;
//     struct block_info *next;
//     size_t block_size;
// };

typedef struct block_info{
    struct block_info* prev;
    struct block_info* next;
    void* mem_ptr; // denotes where the header of the block is
    size_t block_size; // tracks the size of the block
    size_t isFree; // 0=not free, 1=free
} m_block;

m_block* head; // initialize head/start block
void* heapptr;
#define CHUNKSIZE  (1<<12)
// #define max(a, b) ((a) > (b) ? (a) : (b))

/* rounds up to the nearest multiple of ALIGNMENT */
static size_t align(size_t x)
{
    return ALIGNMENT * ((x+ALIGNMENT-1)/ALIGNMENT);
}

/*
 * Initialize: returns false on error, true on success.
 */
bool mm_init(void)
{
    if(mem_sbrk(16) == (void*)-1){
        return false;
    }
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
    }
    // printf("%zu",mem_heapsize());
    // if(size > mem_heapsize()){
    //     heapptr = mem_sbrk(align(size));
    // }

    int     newsize = align(size+4);  // add header(4byte)!
    void*   ptr     = mem_sbrk(newsize);    // ptr must be aligned.
    size_t sbrk_size;
    
    // if(ptr == (void*)-1)
    //     return NULL;

    // Initial case, where headptr is null
    if(heapptr == NULL){
        sbrk_size = align((newsize) > (CHUNKSIZE) ? (newsize) : (CHUNKSIZE));
        if((ptr = mem_sbrk(sbrk_size)) == (void*) -1)
            return NULL;
        *(size_t*)ptr = ((newsize) | (1));
        heapptr = ptr;
        dbg_printf("initial sbrk: %x \n", (unsigned int)sbrk_size);
        dbg_printf("heapsize: %x \n", (unsigned int)mem_heapsize());
    } else{
        sbrk_size = align((newsize) > (CHUNKSIZE) ? (newsize) : (CHUNKSIZE));
        if((ptr = mem_sbrk(sbrk_size)) == (void*) -1)
            return NULL;
        *(size_t*)ptr = ((newsize) | (1)); //PACK(newsize,1); // BLOCK size! NOT PAYLOAD size!
        heapptr = ptr;
        dbg_printf("sbrk: %x \n", (unsigned int)sbrk_size);
                // printf("-origin size = %d \n", size);
                // printf("-aligned size = %d \n", ALIGN(size));
                // printf("-ptr = %p \n", ptr);
                // printf("-in malloc %d \n", GET(ptr));
    }
    return (void*)((char*)ptr); // ptr points payload!
    // else{
    //     void *ptr = mem_sbrk(align(size)); 
    //     if(!ptr){ // indicates mem_sbrk failed
    //         return NULL;
    //     } else{ //endcase, ensures pointer errors caught
    //         return ptr;
    //     }
    // }
    // int fullsize = align(size); // adding 4 for header size
    // void *ptr = mem_sbrk(fullsize); 
    // if(ptr == (void*) -1){
    //     return NULL;
    // } else{
        // m_block* temp = head;
        // m_block* lastvalid = head;

        // while (temp != NULL) {
        //     lastvalid = temp;
        //     temp = temp->next;
        // }

        // // Creates a new block to malloc
        // m_block* newBlock = (m_block*) mem_sbrk(align(size));
        // // mem_sbrk correctly placing memory address 9904 bytes after head
        // if (newBlock == (void*) -1) {
        //     return NULL;
        // }
        // newBlock->block_size = align(size);
        // newBlock->isFree = 0; // set created block to not free.
        // newBlock->mem_ptr = (void *) newBlock; // if add BLOCK_SIZE, throws error that address not aligned.
        // // created block gets mem_ptr value assigned correctly up to here (gdb confirmed)

        // memset(newBlock->mem_ptr, 0, newBlock->block_size); // sets 'size' bytes of memory at pointer (mem_ptr) to 0.
        // newBlock->mem_ptr = (void *) newBlock;

        // lastvalid->next = newBlock; // lastvalid is the head pointer, head->next is create.
        // newBlock->prev = lastvalid; //correctly set to lastvalid address (gdb confirmed)
        // newBlock->next = NULL;

        // // heapchecker v1
        // if(mem_heapsize() >= 90036){
        //     printf("Heap Exceeeded");
        //     return temp;
        // }
        // return newBlock->mem_ptr;
    // }
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
    // void *endptr = mem_sbrk(0); 

    // struct block_info *to_free = (struct block_info*) (((char*) ptr)
    //         - sizeof(struct block_info));
}

/*
 * Project2
 * Function: realloc
 * Description: Resizes memory block at address from pointer that was allocated by malloc/calloc
 * Input: A pointer to a previously allocated memory block, a size value in bytes
 * Output: None
 */
void* realloc(void *oldptr, size_t size)
{
    // void *newptr;
    // size_t copySize;
    
    // newptr = malloc(align(size));
    // if (newptr == NULL)
    //   return NULL;
    // copySize = *(size_t *)((char *)oldptr - sizeof(size_t));
    // if (size < copySize)
    //   copySize = size;
    // memcpy(newptr, oldptr, copySize);
    // mm_free(oldptr);
    // return newptr;

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