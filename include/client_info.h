#ifndef CLIENT_INFO_H
#define CLIENT_INFO_H

#include <sys/types.h>
#include <sys/socket.h>
#include <pthread.h>

#define NAME_MAX 64

struct client_info {
  int cfd;
  char name[NAME_MAX];
  size_t name_len;
  char *partial_msg;
  size_t partial_len;
  size_t partial_cap;
  pthread_t thread;
};

#endif
