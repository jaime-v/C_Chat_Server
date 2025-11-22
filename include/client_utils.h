#ifndef CLIENT_UTILS_H
#define CLIENT_UTILS_H

#include "client_info.h"
#include <stddef.h>

/**
 *  Append payload to buffer stored in client_info struct pointed to by client.
 *
 *  @param client       Pointer to info of client.
 *  @param payload      Message that client has read in.
 *  @param bytes_read   Number of bytes that client has read in.
 *
 *  @return             0 on success, -1 on failure.
 */
int append_to_client_buffer(struct client_info *client, char *payload, size_t bytes_read);

/**
 *  Clear buffer stored in client.
 *
 *  @param client     Pointer to client's info.
 *
 *  @return           0 on success, -1 on failure.
 */
int clear_client_buffer(struct client_info *client);

#endif
