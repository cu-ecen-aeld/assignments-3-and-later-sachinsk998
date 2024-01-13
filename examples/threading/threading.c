#include "threading.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <pthread.h>

// Optional: use these functions to add debug or error prints to your application
#define DEBUG_LOG(msg,...)
//#define DEBUG_LOG(msg,...) printf("threading: " msg "\n" , ##__VA_ARGS__)
#define ERROR_LOG(msg,...) printf("threading ERROR: " msg "\n" , ##__VA_ARGS__)





void* threadfunc(void* thread_param)
{

    // TODO: wait, obtain mutex, wait, release mutex as described by thread_data structure
    // hint: use a cast like the one below to obtain thread arguments from your parameter
    //struct thread_data* thread_func_args = (struct thread_data *) thread_param;
    
	// Cast the thread arguments
    struct thread_data* thread_st = (struct thread_data*) thread_param;

    // Print a debug message
    DEBUG_LOG("Threadfunc started");

    // Wait to obtain the mutex
    usleep(thread_st->wait_to_obtain_ms * 1000);
    
    // Obtain the mutex
    int lock_ret = pthread_mutex_lock(thread_st->thread_mutex);
    if (lock_ret != 0) {
        ERROR_LOG("pthread_mutex_lock failed");
        thread_st->thread_complete_success = false;
        return NULL;
    }

    // Wait to release the mutex
    usleep(thread_st->wait_to_release_ms * 1000);

    // Release the mutex
    int unlock_ret = pthread_mutex_unlock(thread_st->thread_mutex);
    if (unlock_ret != 0) {
        ERROR_LOG("pthread_mutex_unlock failed");
        thread_st->thread_complete_success = false;
        return NULL;
    }

    // Set thread completion flag to true
    thread_st->thread_complete_success = true;

    // Print a debug message
    DEBUG_LOG("Threadfunc done");
    
    
    
    return thread_param;
}


bool start_thread_obtaining_mutex(pthread_t *thread, pthread_mutex_t *mutex,int wait_to_obtain_ms, int wait_to_release_ms)
{
    /**
     * TODO: allocate memory for thread_data, setup mutex and wait arguments, pass thread_data to created thread
     * using threadfunc() as entry point.
     *
     * return true if successful.
     *
     * See implementation details in threading.h file comment block
     */
     
     
     // Allocate memory for thread_data
    struct thread_data *thread_st = (struct thread_data *) malloc(sizeof(struct thread_data));
    if (thread_st == NULL) {
        ERROR_LOG("Not enough memory available");
        return false;
    }
  
     // Initialize thread_data structure
    thread_st->thread_mutex = mutex;
    thread_st->wait_to_obtain_ms = wait_to_obtain_ms;
    thread_st->wait_to_release_ms = wait_to_release_ms;
    thread_st->thread_complete_success = false;
    
       // Create a new thread using pthread_create
    int create_ret = pthread_create(thread, NULL, threadfunc, (void*) thread_st);
    if (create_ret != 0) {
        ERROR_LOG("pthread_create failed");
        free(thread_st);
        return false;
    } else {
        DEBUG_LOG("pthread_create success");
        // Save the thread pointer in the thread_data structure
        thread_st->thread = thread;
        return true;
    }
}

