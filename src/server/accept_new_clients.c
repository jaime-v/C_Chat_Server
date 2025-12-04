#include "accept_new_clients.h"
#include "client_info_init.h"
#include "client_list.h"
#include "common_header.h"
#include "protocol.h"
#include <sys/socket.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <stdlib.h>
#include <stdio.h>

int accept_new_clients(struct server_state *state, int epoll_fd){
  // Apparently we should for loop this? but not sure why
  // Supposedly we can have multiple clients wanting to connect but wouldnt that just cause
  // multiple events?
  // How can one event cause multiple connections?

  for(;;){
    // Accept new client
    int client_fd = accept(state->server_fd, NULL, NULL);
    printf("[DEBUG - accept_new_clients]: Accepted: %d\n", client_fd);

    // Check client errors
    if(client_fd == -1){
      if(errno == EAGAIN || errno == EWOULDBLOCK){
        // We have no more clients to accept
        perror("No more clients to accept");
        return 0;
      } else {
        // We have an actual error
        perror("We have an actual accept error");
        return -1;
      }
    }

    // Set new socket to nonblocking
    if(fcntl(client_fd, F_SETFL, O_NONBLOCK) == -1){
      perror("accept_new_clients - fcntl");
      return -1;
    }

    // Create client_info struct
    struct client_info *info = (struct client_info *)malloc(sizeof(*info));
    if(client_info_init(info, client_fd) == -1){
      perror("accept_new_clients - client_info_init");
      return -1;
    }

    // Add client to epoll interest list
    struct epoll_event ev = {0};
    ev.events = EPOLLIN | EPOLLRDHUP;
    ev.data.ptr = info;
    printf("[DEBUG accept_new_clients]: putting client %d into epoll\n", info->client_fd);
    if(epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &ev) == -1){
      perror("accept_new_clients - epoll_ctl");
      return -1;
    }

    // Add client to server state's global client list
    if(add_client_to_list(state, info) == -1){
      perror("accept_new_clients - add_client_to_list");
      return -1;
    }
    printf("[DEBUG - accept_new_clients]: client_count: %zu\n", state->client_count);
  }
  perror("We should not be able to hit this");
  return -1;
}
