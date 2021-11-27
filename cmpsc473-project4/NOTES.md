## NOTES:
### Commands to Run:
- ./wordcount [test_case_name] [iters]
(Note, if no testname provided, all tests will run, default iterations is 1)
- DO NOT EDIT README.MD unless for performance evaluation.
- test_channel_close_with_send tests the case where you send and recieve and close the buffer simultaneously

# Running ./test.py runs through all tests, but incredibly slow. many errors. Score=15 without changing any code.
# Look at test.py to find name of test, then run ./test.py [test_number] to run test.

## DEBUGGING
- To begin debugging, need to run 'make debug', then use command:
Ex. './wordcount serialize' to run individual test. (use for GDB)

## RUNNING PERFORMANCE EVAL:
'./wordcount custom_eval 2 100 perf_eval.txt perf_out.txt'
- Runs wordcount with input file 'perf-eval.txt' with 2 mapper threads and a 100Byte buffer, outputs results to perf_out.txt

### NOTES:
Theory1: Thread count increasing causes completion time to increase
- As we varied the number of threads in Tests 1-6, the average completion time increased consistently. (found that the increase is linear)
- Compare Test 1-6
- The increase in time could be explained by the handling of the increased of number of threads (ie. making the program ‘thread-safe’), avoiding data-races.
- When you have more threads there is more communication between these threads and the mutexs. Locking and unlocking, creating wait conditions and signals. These all tack on extra time to the completion of the process for the reward that the program is now thread-safe.
- Map reduce is most effective in HPC (high performance computing) applications with distributed computing resources, whereas in our program, the program is run on a single host, where resources are shared.
Theory2: Buffer size increasing doesn’t change completion time
- It looks like the buffer size doesn’t have a large/any impact on the real completion time of the program.
Compare Threads 7-9.


## To do process timing, use linux timer
### https://ostechnix.com/how-to-find-the-execution-time-of-a-command-or-process-in-linux/
- Use 'time ./wordcount [XXX]'

# VERSION HISTORY
## 4/28/21 4:30pm Version
- Currently passing all except closes. But that SHOULD be a minor fix.
- May need to create a second mutex for the close.




