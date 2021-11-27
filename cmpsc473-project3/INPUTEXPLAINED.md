## Input Parameters (Explained):
# list of system parameters specified as parameter-descriptive-string parameter-value; (all latencies are in  units of a “cycle”)
# list of parameters that will remain the same across all experiments
Non-mem-inst-length 1
Virtual-addr-size-in-bits 32
DRAM-size-in-MB 4
TLB-size-in-entries 16
TLB-latency 1
DRAM-latency 100
Swap-latency 10000000
Page-fault-trap-handling-time 10000
Swap-interrupt-handling-time 10000
TLB-type FullyAssociative
TLB-replacement-policy LRU

# list of parameters that may vary across experiments
P-in-bits 12 = Page offset
Frac-mem-inst 0 = fraction of memory instructions in the input file
Num-pagetable-levels 3 = Can be anything, but set to 3 since it is the max page table size we'll use.
N1-in-bits 8 = page table 1 size
N2-in-bits 8 = page table 2 size
N3-in-bits 4 = page table 3 size
Page-replacement-policy LRU
Num-procs 3 = number of processes.
# trace-file-name arrival-time-of-process
process1 0
process2 3
process3 4