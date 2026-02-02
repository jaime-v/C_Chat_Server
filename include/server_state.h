#ifndef SERVER_STATE_H
#define SERVER_STATE_H

#include "client_info.h"
#include <pthread.h>

#define MAX_CLIENTS 1000

struct server_state {
  int server_fd;
  int epoll_fd;
  struct client_info *client_list[MAX_CLIENTS];
  size_t client_count;
  pthread_mutex_t client_mutex;
};

/**
 *  Initializes struct server_state pointed to by state.
 *  state_out is modified internally.
 *
 *  @param state_out    Pointer to server_state struct.
 *
 *  @return             0 on success, -1 on failure.
 */
int server_state_init(struct server_state *state_out);

/**
 *  Cleans up server state. Cleans up each client, destroys the state's mutex,
 * and closes the server file descriptor.
 *
 *  @param state        Pointer to server_state struct.
 *
 *  @return             0 on success, -1 on failure.
 */
int server_state_destroy(struct server_state *state);

#endif
