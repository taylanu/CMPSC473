## To start debugging, use 
gdb --arg ./simulator input1.txt output1.txt


### 4/15/21: 12:01pm Segfault (SOLVED)
Program received signal SIGSEGV, Segmentation fault.
0x0000555555555ddc in diskToMemory () at simulator.c:340
340	    struct NextMem* addr = gll_first(p -> memReq);


