#ifndef CLIENT_TRY_WRITE_H
#define CLIENT_TRY_WRITE_H

#include "client_info.h"

/**
 *  Try to write from the client's message queue to the client file descriptor.
 *
 *  
 */
int client_try_write(int epoll_fd, struct client_info *client);
#endif
