#include "epoll_mod.h"
#include <sys/epoll.h>
#include <stdio.h>

void enable_epollout_for_client(int epoll_fd, struct client_info *client){
  if(client->epollout_enabled == 1){
    return;
  }

  struct epoll_event ev;
  ev.data.ptr = client;
  ev.events = EPOLLIN | EPOLLOUT;
  if(epoll_ctl(epoll_fd, EPOLL_CTL_MOD, client->client_fd, &ev) == 0){
    client->epollout_enabled = 1;
  } else {
    // error
    printf("[DEBUG - epoll_mod]: Error enabling epollout for client %d\n", client->client_fd);
  }
  printf("[DEBUG - epoll_mod]: Enabling epollout for client %d\n", client->client_fd);

}

void disable_epollout_for_client(int epoll_fd, struct client_info *client){
  if(client->epollout_enabled == 0){
    return;
  }

  struct epoll_event ev;
  ev.data.ptr = client;
  ev.events = EPOLLIN;
  if(epoll_ctl(epoll_fd, EPOLL_CTL_MOD, client->client_fd, &ev) == 0){
    client->epollout_enabled = 0;
  } else {
    printf("[DEBUG - epoll_mod]: Error disabling epollout for client %d\n", client->client_fd);
    // error
  }
  printf("[DEBUG - epoll_mod]: Disabling epollout for client %d\n", client->client_fd);


}
