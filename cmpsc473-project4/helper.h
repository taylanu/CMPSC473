#ifndef HELPER_H
#define HELPER_H
#include "queue.h"
//buffer structure. You are allowed to additional members in it.
//    DO NOT CHANGE ANY EXISTING MEMBER
//    DO NOT CHANGE ANY EXISTING MEMBER


typedef struct {
    bool isopen;
    fifo_t* fifoQ;
    pthread_mutex_t chmutex; // Used to lock buffer to prevent data errors
    pthread_mutex_t chclose; // (OPTIONAL)
    pthread_cond_t chconrec; // Used to signal any threads waiting to recieve to start
    pthread_cond_t chconsend; // Used to signal any threads waiting to send to start

} state_t;
#endif