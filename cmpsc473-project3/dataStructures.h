#ifndef DATASTRUCTURES_H
#define DATASTRUCTURES_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "gll.h"


typedef struct PCB ProcessControlBlock;

// Simulated Process Control Block
struct PCB
{
    char* name;
    uint64_t start_time;
    char* memoryFilename;
    FILE* memoryFile;
    gll_t* memReq;
    int numOfIns;
    int hitCount; //TLB
    int missCount; //TLB
    uint64_t fracLeft;


    // STATS
    uint64_t OS_time;
    uint64_t user_time;

    uint64_t pt_hitCount; // PAGE TABLE
    uint64_t pt_missCount; // PAGE TABLE

    uint64_t numberOfDiskInt;
    uint64_t totalTLBmiss;
    uint64_t numberContextSwitch;
    uint64_t numberPageFaults;

} PCBNode;

struct NextMem
{
    char* type;
    char* address;
};


typedef struct Stats{
    char* processName;
    uint64_t hitCount; //TLB
    uint64_t missCount; //TLB
    uint64_t pt_hitCount; // PAGE TABLE
    uint64_t pt_missCount; // PAGE TABLE
    double hitRatio;
    uint64_t duration;
    uint64_t numberOfPgFaults;

    double pageFaultRatio;
    uint64_t numberOfTLBmiss;
    double tlbMissRatio;
    uint64_t blockedStateDuration;
    uint64_t numberOfContextSwitch;
    uint64_t numberOfDiskInt;
    uint64_t totalTLBmiss;
    uint64_t numberContextSwitch;

    uint64_t OS_time;
    uint64_t user_time;
} stats;

typedef struct TotalStats{
    uint64_t start_time;
    uint64_t end_time;
    uint64_t duration;
    gll_t* perProcessStats;
    int numberOfContextSwitch;
    int numberOfDiskInt;
    int totalPgFaults;
    int totalTLBmiss;
    uint64_t totalBlockedStateDuration;
    uint64_t OSModetime;
    uint64_t userModeTime;
    gll_t* executionOrder;

} totalstats;

typedef struct SystemParameters{
    uint64_t non_mem_inst_length;
    int virtual_addr_size_in_bits;
    uint64_t contextSwitchTime;

    uint64_t TLB_latency; // Cache
    uint64_t DRAM_latency; // Memory
    uint64_t Swap_latency; // Disk
    uint64_t Page_fault_trap_handling_time;
    uint64_t Swap_interrupt_handling_time;

    uint64_t quantum;

    int DRAM_size_in_MB;
    int TLB_size_in_entries;
    int P_in_bits;

    char* TLB_replacement_policy;
    char* TLB_type;

    double Frac_mem_inst;
    int Num_pagetable_levels;
    int N1_in_bits;
    int N2_in_bits;
    int N3_in_bits;
    char* Page_replacement_policy;
    int Num_procs;
} systemParameters;

// TLB Data Structure
typedef struct{
    bool empty;
    int access_time;
    int size; // TLB size in bits
    uint8_t mem_address;
} tlb_cache_t;

// Page Table Data Structure
typedef struct{
    bool empty;
    int access_time;
    int size; // page table size in bits
    uint64_t mem_address;
} p_table_t;

// DRAM Data Structure
typedef struct{
    bool empty;
    int access_time;
    int size; // DRAM size in bits
    uint8_t mem_address;
} dram_t;

#endif
