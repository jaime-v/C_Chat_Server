#include "broadcast.h"
#include "protocol.h"
#include <pthread.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <sys/epoll.h>

int broadcast(struct server_state *state, struct client_info *sender, uint8_t *msg, size_t len){
  pthread_mutex_lock(&state->client_mutex);
  for(size_t i = 0; i < state->client_count; ++i){
    if(state->client_list[i] != sender){
      if(state->client_list[i]->closed == 1){
        continue;
      }
      struct msg_header out_header;
      if(create_header(&out_header, len, MSG_TYPE_NORMAL, true) == -1){
        printf("[DEBUG - broadcast]: Couldnt create header\n");
        return -1;
      }

      if(write_header(
          state->client_list[i]->client_fd, 
          (const struct msg_header *)&out_header) == -1){
        printf("[DEBUG - broadcast]: Couldnt write header\n");
        state->client_list[i]->closed = 1;
        continue;
      }

      if(write_payload(
          state->client_list[i]->client_fd,
          (const char *)msg,
          len) == -1){
        printf("[DEBUG - broadcast]: Couldnt write payload\n");
        state->client_list[i]->closed = 1;
        continue;
      }
    }
  }
  pthread_mutex_unlock(&state->client_mutex);
  return 0;
}

int server_send_message(int cfd, uint8_t *msg, size_t len){
  struct msg_header out_header;
  if(create_header(&out_header, len, MSG_TYPE_ADMIN, true) == -1){
    return -1;
  }

  if(write_header(cfd, (const struct msg_header *)&out_header) == -1){
    return -1;
  }

  if(write_payload(cfd, (const char *)msg, len) == -1){
    return -1;
  }

  return 0;
}

int client_send_direct_message(int cfd, uint8_t *msg, size_t msg_len){
  struct msg_header out_header;

  if(create_header(&out_header, msg_len, MSG_TYPE_NORMAL, true) == -1){
    return -1;
  }

  if(write_header(cfd, (const struct msg_header *)&out_header) == -1){
    return -1;
  }

  if(write_payload(cfd, (const char *)msg, msg_len) == -1){
    return -1;
  }

  return 0;
}
