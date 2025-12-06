#include "broadcast.h"
#include "protocol.h"
#include "msg_packet.h"
#include "client_utils.h"
#include "client_try_write.h"
#include "epoll_mod.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/epoll.h>

int broadcast(struct server_state *state, struct client_info *sender, uint8_t *msg, size_t len){
  pthread_mutex_lock(&state->client_mutex);
  // Create message header
  struct msg_header out_header;
  if(create_header(&out_header, len, MSG_TYPE_NORMAL, true) == -1){
    return -1;
  }
  for(size_t i = 0; i < state->client_count; ++i){
    struct client_info *current_client = state->client_list[i];
    if(current_client != sender){
      if(current_client->closed == 1){
        perror("[DEBUG - broadcast]: client already marked for closed, won't broadcast");
        continue;
      }

      // Make message packet
      struct msg_packet *packet = create_packet(&out_header, msg, len);
      if(packet == NULL){
        // Error making packet
        continue;
      }

      // Enqueue the message packet to the client
      if(client_enqueue_msg_packet(current_client, packet, MSG_SIZE_LIMIT) == -1){
        free(packet->packet_data);
        free(packet);
      }

      if(current_client->epollout_enabled == 0 && current_client->msg_queue.head == packet){
        printf("[DEBUG - broadcast]: Client %d does not have epoll enabled and its head is the current packet\n", current_client->client_fd);
        if(client_try_write(state->epoll_fd, current_client) == -1){
          current_client->closed = 1;
        }
      }

      if(current_client->msg_queue.head != NULL){
        enable_epollout_for_client(state->epoll_fd, current_client);
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
