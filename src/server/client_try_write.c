#include "client_try_write.h"
#include "msg_packet.h"
#include "epoll_mod.h"
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>

int client_try_write(int epoll_fd, struct client_info *client){
  printf("[DEBUG - client_try_write]: Entering the write loop\n");
  while(client->msg_queue.head != NULL){
    struct msg_packet *packet = client->msg_queue.head;
    size_t remaining = packet->len - packet->offset;
    ssize_t bytes_written = send(client->client_fd, packet->packet_data + packet->offset, remaining, MSG_NOSIGNAL);
    printf("[DEBUG - client_try_write]: We finished a write to: %d, bytes_written %zu\n", client->client_fd, bytes_written);
    if(bytes_written > 0){
      packet->offset += (size_t)bytes_written;
      client->msg_queue.queued_bytes -= (size_t)bytes_written;
      if(packet->offset == packet->len){
        printf("[DEBUG - client_try_write]: We finished a packet, removing %p\n", (void *)packet);
        client->msg_queue.head = packet->next;
        if(client->msg_queue.head == NULL){
          client->msg_queue.tail = NULL;
        }
        free(packet->packet_data);
        free(packet);
        client->msg_queue.queued_count--;
        continue;
      }
      continue;
    }

    if(bytes_written < 0){
      if(errno == EAGAIN || errno == EWOULDBLOCK){
        printf("[DEBUG - client_try_write]: Got would block error, returning 0 to caller\n");
        // epoll error
        return 0;
      }
      if(errno == EINTR){
        // Interrupt error
        printf("[DEBUG - client_try_write]: Got would interrupt error, continuing\n");
        continue;
      }
      // Actual error
      printf("[DEBUG - client_try_write]: Got actual error, returning -1\n");
      perror("[DEBUG - client_try_write]: error: ");
      return -1;
    }

    // bytes_written == 0
    // Disconnect
    return -1;
  }
  printf("[DEBUG - client_try_write]: Disabling epollout for %d\n", client->client_fd);
  disable_epollout_for_client(epoll_fd, client);
  return 0;
}

