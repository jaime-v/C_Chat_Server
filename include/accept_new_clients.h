#ifndef ACCEPT_NEW_CLIENTS_H
#define ACCEPT_NEW_CLIENTS_H

#include "server_state.h"

int accept_new_clients(struct server_state *state, int epoll_fd);

#endif
