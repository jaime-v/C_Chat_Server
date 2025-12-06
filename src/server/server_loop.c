#include "client_info.h"
#include "common_thread.h"
#include "client_thread_args.h"
#include "client_info_init.h"
#include "client_list.h"
#include "client_try_write.h"
#include "server_control.h"
#include "broadcast.h"
#include "process_payload.h"
#include "accept_new_clients.h"
#include "handle_client_read.h"
#include "utils.h"
#include "client_utils.h"
#include <sys/socket.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <netinet/in.h>
#include <string.h>

// Epoll transition
#include <errno.h>
#include <fcntl.h>
#include <sys/epoll.h>

#define MAX_EVENTS 10

int server_loop(struct server_state *state){
  int nfds;

  struct epoll_event ev = {0};
  struct epoll_event events[MAX_EVENTS];
  ev.events = EPOLLIN;
  // EPOLLIN is when we have a read event
  // EPOLLRDHUP is when we have a read direction being closed -- used when a client exits and 
  //  exits gracefully
  // EPOLLERR and EPOLLHUP is always a part of our events list
  ev.data.fd = state->server_fd;

  if(epoll_ctl(state->epoll_fd, EPOLL_CTL_ADD, state->server_fd, &ev) == -1){
    perror("epoll_ctl");
    return -1;
  }


  while(server_shutdown != 1){
    for(size_t i = 0; i < state->client_count; ++i){
      struct client_info *current_client = state->client_list[i];
      if(current_client->closed == 1){
        printf("[DEBUG - server_loop]: client is closing\n");
        if(epoll_ctl(state->epoll_fd, EPOLL_CTL_DEL, current_client->client_fd, NULL) < 0){
          printf("Epoll error?\n");
        } 
        while(current_client->msg_queue.head != NULL){
          struct msg_packet *next = current_client->msg_queue.head->next;
          free(current_client->msg_queue.head->packet_data);
          free(current_client->msg_queue.head);
          current_client->msg_queue.head = next;
        } 
        remove_client_from_list(state, state->client_list[i]);
      }
    }

    // EPOLL TIME
    // Wait for events on the epoll_fd
    printf("\n\n\nWaiting -- currently have: %zu clients\n\n", state->client_count);
    nfds = epoll_wait(state->epoll_fd, events, MAX_EVENTS, -1);
    if(nfds == -1){
      perror("\n\n\n\n\nepoll_wait\n\n\n\n\n");
      return -1;
    }

    // Once we have file descriptors with new activity, we do something
    for (int i = 0; i < nfds; i++){
      // If the event came from our listening socket, we have a client to accept
      int fd = events[i].data.fd;
      if(fd == state->server_fd){
        if(accept_new_clients(state) == -1){
          perror("server_loop - accept_new_clients");
          continue;
        }
      } else {
        // Otherwise the event is a bitmask for something
        uint32_t new_event = events[i].events;
        struct client_info *info = (struct client_info *)events[i].data.ptr;
        if(info == NULL){
          printf("Somehow the info is null\n");
        }
        if(info->closed == 1){
          continue;
        }

        // If we have EPOLL Error or EPOLL Hangup, disconnect the client
        if(new_event & (EPOLLHUP | EPOLLRDHUP | EPOLLERR)){
          printf("[DEBUG - server_loop]: Disconnect event - marking client %d for closed\n", info->client_fd);
          info->closed = 1;
          continue;
        }
       
        // If we have EPOLLIN event, the socket has data for reading
        if(new_event & EPOLLIN){
          if(handle_client_read(state, info) < 0){
            printf("[DEBUG - server_loop]: handle_client_read returned -1, changing client %d to close\n", info->client_fd);
            info->closed = 1;
            continue;
          }
        }
 
        // If we have EPOLLOUT event, we can write to the socket
        if(new_event & EPOLLOUT){
          printf("[DEBUG - server_loop] Detected EPOLLOUT, trying to write\n");
          if(client_try_write(state->epoll_fd, info) == -1){
            info->closed = 1;
            continue;
          }
        }
      }
    }
  }

  perror("We are shutting down for some reason");
  char *shutdown_message = "SERVER SHUTDOWN";
  size_t shutdown_len = strlen(shutdown_message);
  broadcast(state, NULL, (uint8_t *)shutdown_message, shutdown_len);

  // Shutdown for server
  if(epoll_ctl(state->epoll_fd, EPOLL_CTL_DEL, state->server_fd, NULL) < 0){
    printf("Epoll error?\n");
  }
  shutdown(state->server_fd, SHUT_RDWR);
  if(close(state->server_fd) == -1){
    perror("close error on server_fd");
  }
 
  for(size_t i = 0; i < state->client_count; ++i){
    struct client_info *current_client = state->client_list[i];
    while(current_client->msg_queue.head != NULL){
      struct msg_packet *next = current_client->msg_queue.head->next;
      free(current_client->msg_queue.head->packet_data);
      free(current_client->msg_queue.head);
      current_client->msg_queue.head = next;
    }
  }
  if(remove_all_clients(state) == -1){
    // Which we might not want? we want to retry this
    // return -1;
    perror("remove_all_clients");
  }
  return 0;
}
