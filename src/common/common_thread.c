#include "common_thread.h"

int create_and_detach_thread(void *(*worker_func)(void *), void *args){
  pthread_t thread;
  if(pthread_create(&thread, NULL, worker_func, args) != 0){
    return -1;
  }
  if(pthread_detach(thread) != 0){
    return -1;
  }
  return 0;
}
