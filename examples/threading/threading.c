#include "threading.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

// Optional: use these functions to add debug or error prints to your application
#define DEBUG_LOG(msg,...)
//#define DEBUG_LOG(msg,...) printf("threading: " msg "\n" , ##__VA_ARGS__)
#define ERROR_LOG(msg,...) printf("threading ERROR: " msg "\n" , ##__VA_ARGS__)

void* threadfunc(void* thread_param)
{
    
    // TODO: wait, obtain mutex, wait, release mutex as described by thread_data structure
    // hint: use a cast like the one below to obtain thread arguments from your parameter
    //struct thread_data* thread_func_args = (struct thread_data *) thread_param;

    struct thread_data* thread_st = (struct thread_data*) thread_param;
    DEBUG_LOG("threadfunc started");
    usleep(thread_st->wait_to_obtain_ms*1000);
    DEBUG_LOG("threadfunc wait_to_obtain_ms done");
    int Tlock_ret = pthread_mutex_lock(thread_st->thread_mutex);
    if (Tlock_ret != 0)
    {
         ERROR_LOG("pthread_mutex_lock failed");
         thread_st->thread_complete_success = false;
         return thread_param;
    }

    usleep(thread_st->wait_to_release_ms*1000);
    DEBUG_LOG("threadfunc wait_to_release_ms done");
    int Tunlock_ret = pthread_mutex_unlock(thread_st->thread_mutex);
    if (Tunlock_ret != 0)
    {
         ERROR_LOG("pthread_mutex_unlock failed");
         thread_st->thread_complete_success = false;
         return thread_param;
    }
    thread_st->thread_complete_success = true;
    DEBUG_LOG("threadfunc done");  

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
    struct thread_data* thread_st = (struct thread_data *) malloc (sizeof(struct thread_data));
    if (thread_st == NULL)
    {
       ERROR_LOG("no enough memory available");
       return false;
    }

    thread_st->thread_mutex = mutex;
    thread_st->wait_to_obtain_ms = wait_to_obtain_ms;
    thread_st->wait_to_release_ms = wait_to_release_ms;
    thread_st->thread_complete_success = false;

    int Tcreate_ret = pthread_create(thread, NULL, threadfunc, (void*) thread_st);
    if (Tcreate_ret != 0)
    {
       ERROR_LOG("pthread_create failed");
       free(thread_st);
       return false;
    }
    else{
        DEBUG_LOG("pthread_create success");
        thread_st->thread = thread;
        return true;
    }

}

