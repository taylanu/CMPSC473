#include "simulator.h"
#include <string.h>

// Initialize the Page Table
static p_table_t *pt;
int page_table_entries = 20;
// Initialize the TLB Cache
static tlb_cache_t *tlb;

//Initialize the DRAM
static dram_t *dram;

//Bring in Stats
static stats *stat;
void init()
{
    // GLOBAL VARS
    current_time = 0;
    nextQuanta = current_time + quantum;
    readyProcess = gll_init();
    runningProcess= gll_init();
    blockedProcess = gll_init();

    processList = gll_init();
    traceptr = openTrace(traceFileName);

    sysParam = readSysParam(traceptr); // loaded SystemParameters struct.

    //read traces from trace file and put them in the processList
    struct PCB* temp = readNextTrace(traceptr);
    if(temp == NULL)
    {
        printf("No data in file. Exit.\n");
        exit(1);
    }

    while(temp != NULL)
    {
        gll_pushBack(processList, temp);
        temp = readNextTrace(traceptr);
    }

    //transfer ready processes from processList to readyProcess list
    temp = gll_first(processList);

    while((temp!= NULL) && ( temp->start_time <= current_time))
    {
        struct NextMem* tempAddr;
        temp->memoryFile = openTrace(temp->memoryFilename);
        temp->numOfIns = readNumIns(temp->memoryFile);
        tempAddr = readNextMem(temp->memoryFile);
        while(tempAddr!= NULL)
        {
            gll_pushBack(temp->memReq, tempAddr);
            tempAddr = readNextMem(temp->memoryFile);
        }
        gll_pushBack(readyProcess, temp);
        gll_pop(processList);

        temp = gll_first(processList);
    }

    //TODO: Initialize what you need



    // Runs through each page table entry, initializing each entry as empty.
    //for (int i = 0; i < tlb->size; ++i){
    //    tlb[i]->empty = true; // initializes PTE as empty.
    //}
    //

    pt = calloc(sysParam->TLB_size_in_entries, sizeof(tlb_cache_t));
    pt = calloc(sysParam->DRAM_size_in_MB, sizeof(dram_t));
    pt = calloc(page_table_entries, sizeof(p_table_t));

    // Runs through each page table entry, initializing each entry as empty.
    int i;
    for (i = 0; i < page_table_entries; ++i){
        pt[i].empty = true; // initializes PTE as empty.
    }
    int j;
    for (j = 0; j < sysParam->TLB_size_in_entries; ++j){
        tlb[j].empty = true; // initializes PTE as empty.
    }
    int k;
    for (k = 0; k < sysParam->DRAM_size_in_MB; ++k){
        dram[k].empty = true; // initializes PTE as empty.
    }

}

void finishAll()
{
    if((gll_first(readyProcess)!= NULL) || (gll_first(runningProcess)!= NULL) || (gll_first(blockedProcess)!= NULL) || (gll_first(processList)!= NULL))
    {
        printf("Something is still pending\n");
    }
    gll_destroy(readyProcess);
    gll_destroy(runningProcess);
    gll_destroy(blockedProcess);
    gll_destroy(processList);

//TODO: Anything else you want to destroy

    closeTrace(traceptr);
}

void statsinit()
{
    // statsList = gll_init();
    resultStats.perProcessStats = gll_init();
    resultStats.executionOrder = gll_init();
    resultStats.start_time = current_time;
}

void statsUpdate()
{
    resultStats.OSModetime = OSTime;
    resultStats.userModeTime  = userTime;
    resultStats.numberOfContextSwitch = numberContextSwitch;
    resultStats.end_time = current_time;
}

// NonMemory instructions already handled, need to write code for Memory instructions
//returns 1 on success, 0 if trace ends, -1 if page fault
int readPage(struct PCB* p, uint64_t stopTime)
{
    struct NextMem* addr = gll_first(p->memReq);
    uint64_t timeAvailable = stopTime - current_time;

    if(addr == NULL)
    {
        return 0;
    }
    if(debug == 1)
    {
        printf("Request::%s::%s::\n", addr->type, addr->address);
    }

    // Handler for NONMEM Instructions
    if(strcmp(addr->type, "NONMEM") == 0)
    {
        uint64_t timeNeeded = (p->fracLeft > 0)? p->fracLeft: sysParam->non_mem_inst_length;

        if(timeAvailable < timeNeeded)
        {
            current_time += timeAvailable;
            userTime += timeAvailable;
            p->user_time += timeAvailable;
            p->fracLeft = timeNeeded - timeAvailable;
        }
        else{
            gll_pop(p->memReq);
            current_time += timeNeeded;
            userTime += timeNeeded;
            p->user_time += timeNeeded;
            p->fracLeft = 0;
        }

        if(gll_first(p->memReq) == NULL)
        {
            return 0;
        }
        return 1;
    }
    // Handler for MEM Instructions
        // *NOTE: at 9:45 in project video
        // *First after reading in trace, check in TLB if it is present in TLB,
        // *Else, check in the page table, if not present likely a page fault
        // *Will have to simulate a disk request/swapping request. Stop at this point. Issue disk interrupt.
    else{
        //TODO: for MEM traces

        uint64_t timeNeeded = sysParam->Frac_mem_inst;

        if(timeAvailable < timeNeeded){

        }
        else{
            gll_pop(p->memReq);
            current_time += timeNeeded;
            userTime += timeNeeded;
            p->user_time += timeNeeded;
        }
        if(gll_first(p->memReq) == NULL)
        {
            return 0;
        }
        else{
            // *NOTE: Scans each page table entry, checks if the address is already in the page table.
            // *If address is not in the page table, then page fault, return -1
            // *If address is in the page table, return address.
            int j;
            for(j=0; j < sysParam->TLB_size_in_entries; j++){
                if(tlb->mem_address == addr){
                    // print("Address is in the TLB");
                    stat->hitCount += 1;
                    return 1;
                } else{
                    stat->missCount +=1;
                }
            }

            int i;
            for(i=0; i < page_table_entries; i++){
                if(pt->mem_address == addr){
                    // print("Address is in the page table");
                    return pt->mem_address;
                } else{
                    // print("Page Fault occurred");
                    return -1;
                }
            }
        }

        // printf("Mem trace not handled\n");
        // exit(1);
    }
}

void schedulingRR(int pauseCause)
{
    //move first readyProcess to running
    gll_push(runningProcess, gll_first(readyProcess));
    gll_pop(readyProcess);

    if(gll_first(runningProcess) != NULL)
    {
        current_time = current_time + contextSwitchTime;
        OSTime += contextSwitchTime;
        numberContextSwitch++;
        struct PCB* temp = gll_first(runningProcess);
        gll_pushBack(resultStats.executionOrder, temp->name);
    }
}

//                Case1                    Case2                Case3                 Case4                    Case5
/*runs a process. returns 0 if page fault, 1 if quanta finishes, -1 if traceFile ends, 2 if no running process, 4 if disk Interrupt*/
// Handled: Cases 1, 3, 4 are implemented for us.
// **Need to implement Case2 (Timer Interrupt/Quanta Finishes) and implement Case5 (Disk Interrupt)
int processSimulator()
{
    uint64_t stopTime = nextQuanta;
    int stopCondition = 1;
    if(gll_first(runningProcess)!=NULL)
    {
        // Tells us when the next timer or disk interrupt happens.

        //TODO
        //if(TODO: if there is a pending disk operation in the future)
        //{
            //TODO: stopTime = occurance of the first disk interrupt
            // Update to time of the next disk interrupt if it is smaller than the stopTime.
        //    stopCondition = 4;
        //}

        //^^In real code:

        // The while loop handles the Timer Interrupt. Note the stopTime.
        while(current_time < stopTime)
        {
            int read = readPage(gll_first(runningProcess), stopTime);
            if(debug == 1){
                printf("Read: %d\n", read);
                printf("Current Time %" PRIu64 ", Next Quanta Time %" PRIu64 " %" PRIu64 "\n",current_time, nextQuanta, stopTime);
            }
            // Case3, Tracefile ended, readPage finds no processes to run in file
            if(read == 0)
            {
                return -1;
                break;
            }
            // Case1, if there is a page fault, returns 0
            else if(read == -1) //page fault
            {
                if(gll_first(runningProcess) != NULL)
                {
                    // gll_pushBack places the current runningProcess to the end of the blockedProcess list.
                    gll_pushBack(blockedProcess, gll_first(runningProcess));

                    // gll_pop removes current running process from the the runningProcess list.
                    gll_pop(runningProcess);

                    // May need to add logic for handling page fault, replacing missing data. (ie. diskToMemory())
                    diskToMemory();

                    return 0;
                }
            }
        }
        if(debug == 1)
        {
            printf("Stop condition found\n");
            printf("Current Time %" PRIu64 ", Next Quanta Time %" PRIu64 "\n",current_time, nextQuanta);
        }
        return stopCondition;
    }
    // Case4, where no processes are running, return 2. Endcase of sorts
    if(debug == 1)
    {
        printf("No running process found\n");
    }
    return 2;
}

void cleanUpProcess(struct PCB* p)
{
    //struct PCB* temp = gll_first(runningProcess);
    struct PCB* temp = p;
   //TODO: Adjust the amount of available memory as this process is finishing

    struct Stats* s = malloc(sizeof(stats));
    s->processName = temp->name;
    s->hitCount = temp->hitCount;
    s->missCount = temp->missCount;
    s->user_time = temp->user_time;
    s->OS_time = temp->OS_time;

    s->duration = current_time - temp->start_time;

    gll_pushBack(resultStats.perProcessStats, s);

    gll_destroy(temp->memReq);
    closeTrace(temp->memoryFile);

}

void printPCB(void* v)
{
    struct PCB* p = v;
    if(p!=NULL){
        printf("%s, %" PRIu64 "\n", p->name, p->start_time);
    }
}

void printStats(void* v)
{
    struct Stats* s = v;
    if(s!=NULL){
        double hitRatio = s->hitCount / (1.0* s->hitCount + 1.0 * s->missCount);
        printf("\n\nProcess: %s: \nHit Ratio = %lf \tProcess completion time = %" PRIu64
                "\tuser time = %" PRIu64 "\tOS time = %" PRIu64 "\n", s->processName, hitRatio, s->duration, s->user_time, s->OS_time) ;
    }
}

void printExecOrder(void* v)
{
    char* c = v;
    if(c!=NULL){
        printf("%s\n", c) ;
    }
}


// Disk Interrupt handler, and the time needed from page to go from disk to memory.
// Disk interrupt takes 'Swap-latency time'
void diskToMemory()
{
    // PART1: // TODO: Move requests from disk to memory

    // increment current time by 'Swap-latency time' because a page fault occurred
    current_time += sysParam->Swap_latency;
    struct PCB* p = gll_first(blockedProcess);

    struct NextMem* addr = gll_first(p->memReq);
    int i,j,k;
    int m,n,p;
    ///////////////////////////////////TLB ADDITIONS///////////////////////////////////////////
    int tlbsizeFilled = 0;
    for(m=0; m < sysParam->TLB_size_in_entries; m++){
        if(tlb[m].empty == true){
            tlb[m].mem_address = addr;
            tlb[m].access_time = current_time;
        } else{
            tlbsizeFilled++;
        }
    }
    if(tlbsizeFilled == sysParam->TLB_size_in_entries){
        int tlbleastRecent = (unsigned)!((int)0); // Representing infinity for comparison

        // Finds smallest access time value of the page table entries
        for(n=0; n < sysParam->TLB_size_in_entries; n++){
            if(tlb[n].access_time < tlbleastRecent){
                // Update least recently used
                tlbleastRecent = tlb[n].access_time;
            }
        }

        // Finds the page table entry with that least recently used value

        for(p=0; p < sysParam->TLB_size_in_entries; p++){
            if(tlb[p].access_time == leastRecent){
                // Replacing the address and access time
                tlb[p].mem_address = addr;
                tlb[p].access_time = current_time;

            }
        }
    }





    ///////////////////////////////////PAGE TABLE ADDITIONS/////////////////////////////////////
    // Scan page table to find empty position to place address into page table
    int sizeFilled = 0;
    for(i=0; i < page_table_entries; i++){
        if(pt[i].empty == true){
            pt[i].mem_address = addr;
            pt[i].access_time = current_time;
        } else{
            sizeFilled++;
        }
    }

    // Implement LRU if no spaces available
    // Essentially same as empty case, just with replacement of least recently used value.
    if(sizeFilled == page_table_entries){
        int leastRecent = (unsigned)!((int)0); // Representing infinity for comparison

        // Finds smallest access time value of the page table entries
        for(j=0; j < page_table_entries; j++){
            if(pt[j].access_time < leastRecent){
                // Update least recently used
                leastRecent = pt[j].access_time;
            }
        }

        // Finds the page table entry with that least recently used value

        for(k=0; k < page_table_entries; k++){
            if(pt[k].access_time == leastRecent){
                // Replacing the address and access time
                pt[k].mem_address = addr;
                pt[k].access_time = current_time;

            }
        }
    }

    // PART2: // TODO: move appropriate blocked process to ready process
    // Simply do what was before calling diskToMemory in reverse.
    // The current process is at end of the blockedProcess list.

    // Places the current process from the blockedProcess list to the front of the ready processes list.
    gll_push(readyProcess, gll_popBack(blockedProcess));

    if(debug == 1)
    {
        printf("Done diskToMemory\n");
    }
}


void simulate()
{
    init();
    statsinit();

    //get the first ready process to running state
    struct PCB* temp = gll_first(readyProcess);
    gll_pushBack(runningProcess, temp);
    gll_pop(readyProcess);

    struct PCB* temp2 = gll_first(runningProcess);
    gll_pushBack(resultStats.executionOrder, temp2->name);

    while(1)
    {
        int simPause = processSimulator(); // where process simulator finishes
        if(current_time == nextQuanta)
        {
            nextQuanta = current_time + quantum;
        }

        //transfer ready processes from processList to readyProcess list
        struct PCB* temp = gll_first(processList);

        while((temp!= NULL) && ( temp->start_time <= current_time))
        {
            temp->memoryFile = openTrace(temp->memoryFilename);
            temp->numOfIns = readNumIns(temp->memoryFile);

            struct NextMem* tempAddr = readNextMem(temp->memoryFile);

	        while(tempAddr!= NULL)
            {
                gll_pushBack(temp->memReq, tempAddr);
                tempAddr = readNextMem(temp->memoryFile);
            }
            gll_pushBack(readyProcess, temp);
            gll_pop(processList);

            temp = gll_first(processList);
        }

        //move elements from disk to memory
        diskToMemory();

        //This memory trace done
        if(simPause == -1)
        {
            //finish up this process
            cleanUpProcess(gll_first(runningProcess));
            gll_pop(runningProcess);
        }

        //move running process to readyProcess list
        int runningProcessNUll = 0;
        if(simPause == 1 || simPause == 4)
        {
            if(gll_first(runningProcess) != NULL)
            {
                gll_pushBack(readyProcess, gll_first(runningProcess));
                gll_pop(runningProcess);
            }
            else{
                runningProcessNUll = 1;
            }
            if(simPause == 1)
            {
                nextQuanta = current_time + quantum;
            }
        }

        schedulingRR(simPause);

        //Nothing in running or ready. need to increase time to next timestamp when a process becomes ready.
        if((gll_first(runningProcess) == NULL) && (gll_first(readyProcess) == NULL))
        {
            if(debug == 1)
            {
                printf("\nNothing in running or ready\n");
            }
            if((gll_first(blockedProcess) == NULL) && (gll_first(processList) == NULL))
            {

                    if(debug == 1)
                    {
                        printf("\nAll done\n");
                    }
                    break;
            }
            struct PCB* tempProcess = gll_first(processList);
            struct PCB* tempBlocked = gll_first(blockedProcess);

            //TODO: Set correct value of timeOfNextPendingDiskInterrupt
            uint64_t timeOfNextPendingDiskInterrupt = 0;

            if(tempBlocked == NULL)
            {
                if(debug == 1)
                {
                    printf("\nGoing to move from proess list to ready\n");
                }
                struct NextMem* tempAddr;
                tempProcess->memoryFile = openTrace(tempProcess->memoryFilename);
                tempProcess->numOfIns = readNumIns(tempProcess->memoryFile);
                tempAddr = readNextMem(tempProcess->memoryFile);
                while(tempAddr!= NULL)
                {
                    gll_pushBack(tempProcess->memReq, tempAddr);
                    tempAddr = readNextMem(tempProcess->memoryFile);
                }
                gll_pushBack(readyProcess, tempProcess);
                gll_pop(processList);

                while(nextQuanta < tempProcess->start_time)
                {

                    current_time = nextQuanta;
                    nextQuanta = current_time + quantum;
                }
                OSTime += (tempProcess->start_time-current_time);
                current_time = tempProcess->start_time;
            }
            else
            {
                if(tempProcess == NULL)
                {
                    if(debug == 1)
                    {
                        printf("\nGoing to move from blocked list to ready\n");
                    }
                    OSTime += (timeOfNextPendingDiskInterrupt-current_time);
                    current_time = timeOfNextPendingDiskInterrupt;
                    while (nextQuanta < current_time)
                    {
                        nextQuanta = nextQuanta + quantum;
                    }
                    diskToMemory();
                }
                else if(tempProcess->start_time >= timeOfNextPendingDiskInterrupt)
                {
                    if(debug == 1)
                    {
                        printf("\nGoing to move from blocked list to ready\n");
                    }
                    OSTime += (timeOfNextPendingDiskInterrupt-current_time);
                    current_time = timeOfNextPendingDiskInterrupt;
                    while (nextQuanta < current_time)
                    {
                        nextQuanta = nextQuanta + quantum;
                    }
                    diskToMemory();
                }
                else{
                    struct NextMem* tempAddr;
                    if(debug == 1)
                    {
                        printf("\nGoing to move from proess list to ready\n");
                    }
                    tempProcess->memoryFile = openTrace(tempProcess->memoryFilename);
                    tempProcess->numOfIns = readNumIns(tempProcess->memoryFile);
                    tempAddr = readNextMem(tempProcess->memoryFile);
                    while(tempAddr!= NULL)
                    {
                        gll_pushBack(tempProcess->memReq, tempAddr);
                        tempAddr = readNextMem(tempProcess->memoryFile);
                    }
                    gll_pushBack(readyProcess, tempProcess);
                    gll_pop(processList);

                    while(nextQuanta < tempProcess->start_time)
                    {
                        current_time = nextQuanta;
                        nextQuanta = current_time + quantum;
                    }
                    OSTime += (tempProcess->start_time-current_time);
                    current_time = tempProcess->start_time;
                }
            }
        }
    }
}

int main(int argc, char** argv)
{
    if(argc == 1)
    {
        printf("No file input\n");
        exit(1);
    }
    traceFileName = argv[1];
    outputFileName = argv[2];

    simulate();
    finishAll();
    statsUpdate();

    if(writeToFile(outputFileName, resultStats) == 0)
    {
        printf("Could not write output to file\n");
    }
    printf("User time = %" PRIu64 "\nOS time = %" PRIu64 "\n", resultStats.userModeTime, resultStats.OSModetime);
    printf("Context switched = %d\n", resultStats.numberOfContextSwitch);
    printf("Start time = 0\nEnd time =%llu", current_time);
    gll_each(resultStats.perProcessStats, &printStats);

    // printf("\nExec Order:\n");
    // gll_each(resultStats.executionOrder, &printExecOrder);
    printf("\n");
}
