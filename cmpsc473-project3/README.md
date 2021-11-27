# CMPSCProject3
### Taylan Unal and Joseph Brauckmann

## Command to run simulator:
NONMEM Instructions ONLY:
- ./simulator input1.txt output1.txt
- ./simulator input10.txt output10.txt

MEM Instructions ONLY:
- ./simulator input2.txt output2.txt (pass)
- ./simulator input3.txt output3.txt (pass)
- ./simulator input4.txt output4.txt (fails segfault)
- ./simulator input5.txt output5.txt (pass)
- ./simulator input6.txt output6.txt (infinite loop) [null nonmems running]
- ./simulator input7.txt output7.txt (infinite loop) [null nonmems running]
- ./simulator input8.txt output8.txt (infinite loop) [null nonmems running]
- ./simulator input9.txt output9.txt (infinite loop) [null nonmems running]

CUSTOM TESTS:
- ./simulator /performance/input-custom5.txt output-custom5.txt
- ./simulator /performance/input-custom6.txt output-custom6.txt
- ./simulator /performance/input-custom7.txt output-custom7.txt
- ./simulator /performance/input-custom8.txt output-custom8.txt
- ./simulator /performance/input-custom9.txt output-custom9.txt


## Notes:
1st Priority:
- Create page tables.
- Page table is filled like a cache: first page fault, then pull from lower source (disk)

- TLBs are flushed on every context switch (next process started)

- Timer interrupt means quanta has ended.
- All time units are measured in 'cycles' or units of time, don't think in seconds, ns, etc.
- All page table entries must be aligned to 4 Bytes
- All implementations should be in the simulator.c file
- Non MEM requests have already been handled. Need to write code to handle MEMory requests.
- If a timer interrupt happens, need to update the time,
- If a disk interrupt happens, need to move something from disk to memory (diskToMemory())
- Think of diskToMemory() as page fault handling:

## Requirements:
- Append to simulator handlers for:
1. per-process completion time
2. total number of context switches
3. total number of disk interrupts
4. per-process and total number of TLB misses (absoluteand ratios)
5. per-process and total number of page faults (absoluteand ratios)
6. per-process and total fraction of time in blockedstate
7. total amount of time spent in OS vs. total amountof time spent in user mode.

## Order of Implementation:
1. MMU, DRAM, swap device, Page table only with no TLB
2. Add TLB
3. Performance evaluation.

### Project Description:
Please find the project description here: https://docs.google.com/document/d/14PkFF4idvTlucsVULVwUkChZxvG78FIRbToA-yCL4MM/edit?ts=604bd3e6
To access project description, have to login to PSU Google Account
