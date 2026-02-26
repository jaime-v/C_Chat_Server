#include "client_list.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include <unistd.h>

int add_client_to_list(struct server_state *state, struct client_info *client) {
  pthread_mutex_lock(&state->client_mutex);
  if (state->client_count < MAX_CLIENTS) {
    state->client_list[state->client_count++] = client;
  } else {
    printf("Max Clients Reached\n");
    pthread_mutex_unlock(&state->client_mutex);
    close(client->client_fd);
    free(client);
    return -1;
  }
  pthread_mutex_unlock(&state->client_mutex);
  return 0;
}

int remove_client_from_list(struct server_state *state,
                            struct client_info *client) {
  pthread_mutex_lock(&state->client_mutex);
  for (size_t i = 0; i < state->client_count; i++) {
    if (state->client_list[i] == client) {
      state->client_list[i] = state->client_list[--state->client_count];
      pthread_mutex_unlock(&state->client_mutex);
      if (cleanup_client(client) == -1) {
        return -1;
      }
      return 0;
    }
  }
  return -1;
}

int cleanup_client(struct client_info *client) {
  printf("[INFO: cleanup_client]: Cleaning up client: %d\n", client->client_fd);
  if (!client) {
    return -1;
  }
  if (client->client_fd >= 0) {
    if (close(client->client_fd) == -1) {
      return -1;
    }
  }
  free(client->partial_msg);
  free(client);
  return 0;
}

int remove_all_clients(struct server_state *state) {
  pthread_mutex_lock(&state->client_mutex);
  for (int i = (int)state->client_count - 1; i >= 0; --i) {
    epoll_ctl(state->epoll_fd, EPOLL_CTL_DEL, state->client_list[i]->client_fd,
              NULL);
    if (cleanup_client(state->client_list[i]) == -1) {
      return -1;
    }
  }
  pthread_mutex_unlock(&state->client_mutex);
  return 0;
}
