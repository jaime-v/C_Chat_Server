#include "client_info.h"
#include "common_thread.h"
#include "handle_client.h"
#include "client_thread_args.h"
#include "client_info_init.h"
#include "client_list.h"
#include "server_control.h"
#include "broadcast.h"
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
  int nfds, cfd;

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


  for(;;){
    // EPOLL TIME
    // Wait for events on the epoll_fd
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
        struct client_info *info = events[i].data.ptr;

        // If we have EPOLLIN event, the socket has data for reading
        if(new_event & EPOLLIN){
          while(1){
            if(info->state == READ_HEADER){
              ssize_t bytes_read = read(info->client_fd,
                                        info->header_buffer + info->header_bytes_read,
                                        sizeof(struct client_info) - info->header_bytes_read);
              if(bytes_read == 0){
                // Client closed connection cleanly
                // Remove client
                perror("Client closed connection");
                break;
              }

              if(bytes_read < 0){
                if(errno == EAGAIN || errno == EWOULDBLOCK){
                  // No more data to read
                  perror("client fd would block")
                  break;
                } else {
                  perror("Read header failed");
                  break;
                  // Disconnect client because we failed to read header
                }
              }

              info->header_bytes_read += bytes_read;

              if(info->header_bytes_read < sizeof(struct client_info)){
                // Exit early because we need more header bytes
                perror("Too little header bytes!");
                break;
              }

              if(info->header_bytes_read > sizeof(struct client_info)){
                // Exit early because we need more header bytes
                perror("Too many header bytes! -- how");
                break;
              }

              // If we reach this, then the full header is received and we can typecast
              struct msg_header *header = (struct msg_header *)info->header_buffer;

              info->expected_payload_len = header->msg_len;
              info->msg_type = header->msg_type;
              info->msg_done = header->msg_done;

              info->payload_bytes_read = 0;

              if(info->expected_payload_len > BUF_SIZE){
                perror("expected_payload_len > BUF_SIZE somehow");
              }

              info->state = READ_PAYLOAD;
            } else if(info->state == READ_PAYLOAD){
              ssize_t bytes_read = read(info->client_fd,
                                        info->payload_buffer + info->payload_bytes_read,
                                        info->expected_payload_len - info->payload_bytes_read);

              if(bytes_read == 0){
                // Payload is 0
              }
              
              if(bytes_read < 0){
                if(errno == EAGAIN || errno == EWOULDBLOCK){
                  // No more data to read
                } else {
                  perror("Read payload");
                  // Disconnect because we failed to read payload
                }
              }

              info->payload_bytes_read += bytes_read;

              if(info->payload_bytes_read < info->expected_payload_len){
                // Exit early because we need more payload bytes
                perror("Need more payload bytes");
                break;
              }

              if(info->payload_bytes_read > info->expected_payload_len){
                perror("Somehow read more payload bytes than needed");
                break;
              }

              // If we reach this, then we have read the entire payload
              if(append_to_client_buffer(info) == -1){
                perror("Couldn't append to client buffer");
                break;
              }
      
              if(info->msg_done){
                // If message is done, then we can process it
                // process_message
                char *payload_copy = copy_buffer((char *)info->partial_msg, info->partial_len);
                process_payload(info, (uint8_t *)payload_copy);
                free(payload_copy);
              }

              info->header_bytes_read = 0;
              info->state = READ_HEADER;
            } else {
              perror("How do we read, but not read header or payload?");
            }
          }

        }

        // If we have EPOLLOUT event, we can write to the socket
        // Although, EPOLLOUT triggers every loop since most sockets are writable
        if(new_event & EPOLLOUT){
          handle_client_write(epoll_fd, fd, state);
        }

        // If we have EPOLL Error or EPOLL Hangup, disconnect the client
        if(new_event & (EPOLLHUP | EPOLLRDHUP | EPOLLERR)){
          disconnect_client(epoll_fd, fd, state);
          // Remove client from epoll
          // close the file descriptor
          // Remove client from client list
          // Free client info
          // Which is basically our disconnect protocol already
        }
      }
    }
  }

  char *shutdown_message = "SERVER SHUTDOWN";
  size_t shutdown_len = strlen(shutdown_message);
  broadcast(state, NULL, shutdown_message, shutdown_len);

  if(remove_all_clients(state) == -1){
    // Which we might not want? we want to retry this
    // return -1;
    perror("remove_all_clients");
  }

  printf("[server_loop] holy shit we reached the end\n");
  return 0;
}
