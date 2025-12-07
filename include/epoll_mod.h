#ifndef EPOLL_MOD_H
#define EPOLL_MOD_H

#include "client_info.h"

void enable_epollout_for_client(int epoll_fd, struct client_info *client);
void disable_epollout_for_client(int epoll_fd, struct client_info *client);


#endif
