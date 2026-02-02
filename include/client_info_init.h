#ifndef CLIENT_INFO_INIT_H
#define CLIENT_INFO_INIT_H

#include "client_info.h"

/**
 * Initialize the client info pointed to by client_out.
 *
 * @param client_out  Pointer to struct to store the client info.
 * @param client_fd   File descriptor that is associated with a particular
 * client.
 *
 * @return            0 on success, -1 on failure
 */
int client_info_init(struct client_info *client_out, int client_fd);

#endif
