#ifndef COMMON_THREAD_H
#define COMMON_THREAD_H

#include <pthread.h>

/**
 *  Creates thread with worker_func and args. Immediately detachs thread.
 *
 *  @param worker_func    Function pointer that will be run in the spawned thread.
 *  @param args           Args to pass into the threads worker function.
 *
 *  @return               0 on success, -1 on failure.
 */
int create_and_detach_thread(void *(*worker_func)(void *), void *args);

#endif
