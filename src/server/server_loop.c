#include "client_info.h"
#include "common_thread.h"
#include "client_thread_args.h"
#include "client_info_init.h"
#include "client_list.h"
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

  // EPOLL
  int epoll_fd = epoll_create1(0);
  if(epoll_fd == -1){
    perror("epoll_create1");
    return -1;
  }

  struct epoll_event ev = {0};
  struct epoll_event events[MAX_EVENTS];
  ev.events = EPOLLIN;
  // EPOLLIN is when we have a read event
  // EPOLLRDHUP is when we have a read direction being closed -- used when a client exits and 
  //  exits gracefully
  // EPOLLERR and EPOLLHUP is always a part of our events list
  ev.data.fd = state->server_fd;

  if(epoll_ctl(epoll_fd, EPOLL_CTL_ADD, state->server_fd, &ev) == -1){
    perror("epoll_ctl");
    return -1;
  }


  while(server_shutdown != 1){
    // EPOLL TIME
    // Wait for events on the epoll_fd
    printf("\n\n\nWaiting -- currently have: %zu clients\n\n", state->client_count);
    nfds = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
    if(nfds == -1){
      perror("epoll_wait");
      return -1;
    }

    // Once we have file descriptors with new activity, we do something
    for (int i = 0; i < nfds; i++){
      // If the event came from our listening socket, we have a client to accept
      int fd = events[i].data.fd;
      if(fd == state->server_fd){
        if(accept_new_clients(state, epoll_fd) == -1){
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
          printf("[DEBUG - server_loop]: client is closing\n");
          if(epoll_ctl(epoll_fd, EPOLL_CTL_DEL, info->client_fd, NULL) < 0){
            printf("Epoll error?\n");
          }
          remove_client_from_list(state, info);
          continue;
        }

        // If we have EPOLL Error or EPOLL Hangup, disconnect the client
        if(new_event & (EPOLLHUP | EPOLLRDHUP | EPOLLERR)){
          perror("[DEBUG - server_loop]: WE HAVE A DISCONNECT");
          info->closed = 1;
          continue;
          // disconnect_client(epoll_fd, fd, state);
          // Remove client from epoll
          // close the client file descriptor
          // Remove client from client list
          // Free client info
          // Which is basically our disconnect protocol already
        }
       
        // If we have EPOLLIN event, the socket has data for reading
        if(new_event & EPOLLIN){
          // Probably make this into a function, then if we get a bad return or disconect, we
          // shutdown -- if we get good return, we can move on
          int read_status = 0;
          if((read_status = handle_client_read(state, info)) < 0){
            if(read_status == -1){
              printf("[DEBUG - server_loop]: handle_client_read returned -1\n");
              info->closed = 1;
              continue;
            }
          }
        }
 
        /*
        // If we have EPOLLOUT event, we can write to the socket
        // Although, EPOLLOUT triggers every loop since most sockets are writable
        if(new_event & EPOLLOUT){
          handle_client_write(epoll_fd, fd, state);
        }
        */
      }
    }
  }

  char *shutdown_message = "SERVER SHUTDOWN";
  size_t shutdown_len = strlen(shutdown_message);
  broadcast(state, NULL, (uint8_t *)shutdown_message, shutdown_len);

  // Shutdown for server
  if(epoll_ctl(epoll_fd, EPOLL_CTL_DEL, state->server_fd, NULL) < 0){
    printf("Epoll error?\n");
  }
  shutdown(state->server_fd, SHUT_RDWR);
  if(close(state->server_fd) == -1){
    perror("close error on server_fd");
  }

  if(remove_all_clients(state, epoll_fd) == -1){
    // Which we might not want? we want to retry this
    // return -1;
    perror("remove_all_clients");
  }
  return 0;
}
