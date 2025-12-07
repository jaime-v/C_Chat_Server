#include "server_state.h"
#include "server_control.h"
#include "client_list.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/epoll.h>


#include <stdio.h> // DEBUG

int server_state_init(struct server_state *state_out){
  if(!state_out) {
    return -1;
  }
  memset(state_out, 0, sizeof(*state_out));
  if(pthread_mutex_init(&state_out->client_mutex, NULL) != 0){
    return -1;
  }
  state_out->server_fd = -1;
  state_out->client_count = 0;
  state_out->epoll_fd = epoll_create1(0);
  if(state_out->epoll_fd == -1){
    return -1;
  }
  return 0;
}

int server_state_destroy(struct server_state *state){
  if(!state){
    return -1;
  }
  if(pthread_mutex_destroy(&state->client_mutex) != 0){
    return -1;
  }

  return 0;
}
