## Project2 CMPSC473 SP21
# malloclab
Carefully read malloclab.pdf in its entirety.

## NOTES
- brk(): sets the break value to address, changes allocated space accordingly
- sbrk(): sets

## Checkpoint 2 Targets
### Space Utilization:
Space utilization (60 pts): The space utilization is calculated based on the peak ratio between the aggregate amount of memory used by the driver (i.e., allocated via malloc or realloc but not yet freed via free) and the size of the heap used by your allocator. You should design good policiesto minimize fragmentation in order to increase this ratio.

### Throughput:
Throughput (40 pts): The throughput is a performance metric that measures the average number of operations completed per second. As the performance of your code can vary between executions and between machines, your score as you’re testing your code is not guaranteed. The performance testing will be performed on the W204 cluster machines to ensure more consistent results.

### Heap Checker Examples
- Is every block in the free list marked as free?
- Are there any contiguous free blocks that somehow escapedcoalescing? • Is every free block actually in the free list?
- Do the pointers in the free list point to valid free blocks?
- Do any allocated blocks overlap?
- Do the pointers in a heap block point to valid heap addresses?

## TODO:
- Combine mm.h into mm.c file (done)
- Modify realloc to avoid using memmove, replace by memcpy. (done)
- Create an explicit free list.
- Adapt malloc to perform correctly (keeping as is)
- Perform extensive debugging.
- Consider using the aligned() function to debug.