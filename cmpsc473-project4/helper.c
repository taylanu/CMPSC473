#include "buffer.h"

// Creates a buffer with the given capacity
state_t* buffer_create(int capacity)
{
    state_t* buffer = (state_t*) malloc(sizeof(state_t));
    buffer->fifoQ = (fifo_t *) malloc ( sizeof (fifo_t));
    fifo_init(buffer->fifoQ,capacity);
    buffer->isopen = true;

    // Initialize Mutex variables
    pthread_mutex_init(&buffer->chmutex, NULL); // PTHREAD_MUTEX_INITIALIZER used for static initialization.
    pthread_mutex_init(&buffer->chclose,NULL);

    // Initialize Conditional variables
    pthread_cond_init(&buffer->chconrec,NULL);
    pthread_cond_init(&buffer->chconsend,NULL);

    return buffer;
}

// Writes data to the given buffer
// This is a blocking call i.e., the function only returns on a successful completion of send
// In case the buffer is full, the function waits till the buffer has space to write the new data
// Returns BUFFER_SUCCESS for successfully writing data to the buffer,
// CLOSED_ERROR if the buffer is closed, and
// BUFFER_ERROR on encountering any other generic error of any sort
enum buffer_status buffer_send(state_t *buffer, void* data)
{
    if(!buffer->isopen)
    {
        return CLOSED_ERROR;
    }
    int msg_size = get_msg_size(data);

    // Once buffer_send is called, we have to lock to ensure that multiple threads dont operate on same data.
    pthread_mutex_lock(&buffer->chmutex); // WORKING POSITION FOR LOCK

    // While buffer is smaller than the message size, wait.
    while((fifo_avail_size(buffer->fifoQ) <= msg_size) && buffer->isopen){
        pthread_cond_wait(&buffer->chconsend, &buffer->chmutex);
    }

    // Buffer is available to recieve message data, will unlock the process for next threads to operate.
    if((fifo_avail_size(buffer->fifoQ) > msg_size) && buffer->isopen){
	    buffer_add_Q(buffer,data);

        // cond_signal will wake thread waiting to recieve once sending completed
        pthread_cond_signal(&buffer->chconrec); // cond_signal will wake thread waiting to send once recieving completed

        pthread_mutex_unlock(&buffer->chmutex); // USE STEP IN GDB TO TRACE FILLING BUFFER AFTER THIS POINT
    	return BUFFER_SUCCESS;
    } else{
        pthread_mutex_unlock(&buffer->chmutex); // even if buffer error, still unlock mutex
        return CLOSED_ERROR; //was BUFFER_ERROR
    }

    return BUFFER_ERROR; // END case, other generic error. Needs to be in else.
}

// Reads data from the given buffer and stores it in the function’s input parameter, data (Note that it is a double pointer).
// This is a blocking call i.e., the function only returns on a successful completion of receive
// In case the buffer is empty, the function waits till the buffer has some data to read
// Return BUFFER_SPECIAL_MESSSAGE for successful retrieval of special data "splmsg"
// Returns BUFFER_SUCCESS for successful retrieval of any data other than "splmsg"
// CLOSED_ERROR if the buffer is closed, and
// BUFFER_ERROR on encountering any other generic error of any sort
enum buffer_status buffer_receive(state_t* buffer, void** data)
{
    if(!buffer->isopen)
    {
        return CLOSED_ERROR;
    }

    pthread_mutex_lock(&buffer->chmutex); // WORKING POSITION FOR LOCK

    while((buffer->fifoQ->avilSize >= buffer->fifoQ->size) && buffer->isopen){
        pthread_cond_wait(&buffer->chconrec, &buffer->chmutex);
    }

    if((buffer->fifoQ->avilSize < buffer->fifoQ->size) && buffer->isopen)  // checking if there is something in the Q to remove
    {
    	buffer_remove_Q(buffer,data);
    	if(strcmp(*(char**)(data),"splmsg") ==0)
    	{
            pthread_mutex_unlock(&buffer->chmutex); // even if special message unlock
            pthread_cond_signal(&buffer->chconsend);
        	return BUFFER_SPECIAL_MESSSAGE;
    	}

        pthread_cond_signal(&buffer->chconsend); // cond_signal will wake thread waiting to send once recieving completed
        pthread_mutex_unlock(&buffer->chmutex);
    	return BUFFER_SUCCESS;
    } else{
        // END case, other generic error. Needs to be in else.
        pthread_mutex_unlock(&buffer->chmutex); // even if buffer error, still unlock mutex
        return CLOSED_ERROR;
    }
    return BUFFER_ERROR;
}

// Closes the buffer and informs all the blocking send/receive/select calls to return with CLOSED_ERROR
// Once the buffer is closed, send/receive/select operations will cease to function and just return CLOSED_ERROR
// Returns BUFFER_SUCCESS if close is successful,
// CLOSED_ERROR if the buffer is already closed, and
// BUFFER_ERROR in any other error case
enum buffer_status buffer_close(state_t* buffer)
{ // CURRENTLY ONLY CLOSE FAILING.

    // ORIG CODE
    pthread_mutex_lock(&buffer->chmutex);

    if(!buffer->isopen){
        pthread_mutex_unlock(&buffer->chmutex); // even if closed error, still unlock mutex
        return CLOSED_ERROR;
    } else{
        // This case implies that the buffer is open
        // Wake up all threads waiting to send or recieve
        pthread_cond_broadcast(&buffer->chconsend);
        pthread_cond_broadcast(&buffer->chconrec);

        buffer->isopen = false; // this operation 'closes' the buffer.

        pthread_mutex_unlock(&buffer->chmutex);
        return BUFFER_SUCCESS;
    }
    pthread_mutex_unlock(&buffer->chmutex); // even if buffer error, still unlock mutex
    return BUFFER_ERROR;
}

// Frees all the memory allocated to the buffer , using own version of sem flags
// The caller is responsible for calling buffer_close and waiting for all threads to finish their tasks before calling buffer_destroy
// Returns BUFFER_SUCCESS if destroy is successful,
// DESTROY_ERROR if buffer_destroy is called on an open buffer, and
// BUFFER_ERROR in any other error case
enum buffer_status buffer_destroy(state_t* buffer)
{
    if(buffer->isopen)
    {
        return DESTROY_ERROR;
    }

    // Clears the queue of processes to run.
    fifo_free(buffer->fifoQ);

    // Destroy Mutex Variables
    pthread_mutex_destroy(&buffer->chmutex);
    pthread_mutex_destroy(&buffer->chclose);

    // Destroy Conditional Variables
    pthread_cond_destroy(&buffer->chconrec);
    pthread_cond_destroy(&buffer->chconsend);

    // After destroying elements of buffer, free buffer allocated memory.
    free(buffer);

    return BUFFER_SUCCESS;
}