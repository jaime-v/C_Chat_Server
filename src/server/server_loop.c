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

  struct epoll_event ev, events[MAX_EVENTS];
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
      if(fd == server_fd){
        // function for later
        // accept_new_client(state, epoll_fd);

        // call accept
        int client_fd;
        while((client_fd = accept(state->server_fd, NULL, NULL) != -1)){
          // Do thing
          // set new socket to nonblocking
          fcntl(client_fd, F_SETFL, O_NONBLOCK);

          // Create client_info struct
          struct client_info *info = (struct client_info *)malloc(sizeof(struct client_info));
          if(client_info_init(info, cfd) == -1){
            perror("client_info_init");
            return -1;
          }

          // Add client_fd to epoll
          struct epoll_event ev = {0};
          ev.events = EPOLLIN | EPOLLRDHUP;
          ev.data.ptr = info; // Using ev.data.ptr changes what information epoll returns to us
                              // if we just had ev.data.fd = client_fd, all we would get is the 
                              // file descriptor, but we want to have the whole struct
                              // We need to read state, check how many bytes we read, everything
                              // epoll still monitors the fd, but it gives us the whole struct
                              // to work with
          epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &ev);

          // Add client to global client list
          if(add_client_to_list(state, info) == -1){
            printf("Couldn't add client\n");
          }
        }

        // accept got a -1
        if(client_fd == -1){
          printf("[DEBUG - server_loop] failed\n");
          // Check if errno == EAGAIN
          if(errno == EAGAIN || errno == EWOULDBLOCK){
            // This means that the socket is marked nonblocking and no
            // connections are present to be accepted
            // This signals that no more clients are waiting to be accepted
          } else {
            perror("accept");
            continue;
            // We have a real error in this case
          }
        }
      } else {
        // Otherwise the event is a bitmask for something
        uint32_t new_event = events[i].events;
        struct client_info *info = events[i].data.ptr;

        // If we have EPOLLIN event, the socket has data for reading
        if(new_event & EPOLLIN){
          // handle_client_read(epoll_fd, fd, state);
          // Instead of looping read_header, read_payload forever, we only read as much as
          // available and resume reading on the next EPOLLIN event, if the message hasn't
          // completed yet
          // Once we have a full message, we do the same thing
          //  process the message
          //    store clients name
          //    process commands
          //    broadcast messages
          
          // Read from the cfd into a buffer
          // if bytes_read > 0, process
          // if bytes_read == 0, possibly payload of 0
          // if errno == EAGAIN, then we are good, no more reads for now
          // maybe check for errors.
          while(1){
            if(info->state == READ_HEADER){
              ssize_t bytes_read = read(info->client_fd,
                                        info->header_buffer + info->header_bytes_read,
                                        sizeof(struct client_info) - info->header_bytes_read);
              if(bytes_read == 0){
                // Client closed connection cleanly
                // Remove client
              }

              if(bytes_read < 0){
                if(errno == EAGAIN || errno == EWOULDBLOCK){
                  // No more data to read
                } else {
                  perror("Read header");
                  // Disconnect client because we failed to read header
                }
              }

              info->header_bytes_read += bytes_read;

              if(info->header_bytes_read < sizeof(struct client_info)){
                // Exit early because we need more header bytes
              }

              // If we reach this, then the full header is received and we can typecast
              struct msg_header *header = (struct msg_header *)info->header_buffer;

              info->expected_payload_len = header->msg_len;
              info->msg_type = header->msg_type;
              info->msg_done = header->msg_done;

              info->payload_bytes_read = 0;

              if(info->expected_payload_len > info->payload_cap){
                info->payload_buffer = realloc(info->payload_buffer, info->expected_payload_len);
                info->payload_cap = info->expected_payload_len;
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
              }
      
              // If we reach this then we process the message
              if(info->msg_done){
                // If message is done, then we can process it
                // process_message
                info->payload_bytes_read = 0;
              }

              info->header_bytes_read = 0;
              info->state = READ_HEADER;

            } else {
              perror("What");
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


    // Accept incoming client
    // We don't need the client address or its length, so we can NULL those
    cfd = accept(state->server_fd, NULL, NULL);
    if(cfd == -1){
      printf("[server_loop] Accept failed errno: %d\n", errno);
      if(server_shutdown == 1){
        printf("[server_loop] FOUND SHUTDOWN FLAG %p = %d\n", (void *)&server_shutdown, 
            server_shutdown);
        break;
      } else {
        printf("[server_loop] Continuing then\n");
        continue;
      }
      // return -1;
      // We don't really want to return -1 on a failure, we want to continue trying to accept
      // This error is not worthy of a shutdown
    }

    // Create client_info struct
    struct client_info *info = (struct client_info *)malloc(sizeof(struct client_info));
    if(client_info_init(info, cfd) == -1){
      perror("client_info_init");
      return -1;
    }

    // Create client's thread
    struct client_thread_args *args = malloc(sizeof(struct client_thread_args));
    args->state = state;
    args->info = info;
    if(pthread_create(&info->thread, NULL, handle_client, (void *)args) != 0){
      perror("pthread_create");
      return -1;
    }
    /*
    if(create_and_detach_thread(handle_client, (void *)args) == -1){
      return -1;
    }
    */

    if(add_client_to_list(state, info) == -1){
      printf("Couldn't add client\n");
    }
  }

  char *shutdown_message = "SERVER SHUTDOWN";
  size_t shutdown_len = strlen(shutdown_message);
  broadcast(state, NULL, shutdown_message, shutdown_len);

  for(size_t i = 0; i < state->client_count; i++){
    shutdown(state->client_list[i]->cfd, SHUT_RDWR);
    pthread_join(state->client_list[i]->thread, NULL);
  }

  if(remove_all_clients(state) == -1){
    // Which we might not want? we want to retry this
    // return -1;
    perror("remove_all_clients");
  }

  printf("[server_loop] holy shit we reached the end\n");
  return 0;
}
