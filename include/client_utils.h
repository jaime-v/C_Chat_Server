#ifndef CLIENT_UTILS_H
#define CLIENT_UTILS_H

#include "client_info.h"
#include <stddef.h>

/**
 *  Append payload to buffer stored in client_info struct pointed to by client.
 *
 *  @param client       Pointer to info of client.
 *
 *  @return             0 on success, -1 on failure.
 */
int append_to_client_buffer(struct client_info *client);

/**
 *  Clear buffer stored in client.
 *
 *  @param client     Pointer to client's info.
 *
 *  @return           0 on success, -1 on failure.
 */
int clear_client_buffer(struct client_info *client);

/**
 *  Copies name (stored in client's buffer) into client's name field.
 *  If length of name > NAME_MAX, name is truncated to the first NAME_MAX bytes.
 *
 *  @param client   Pointer to client's info struct.
 *
 *  @return         0 on success -- might be void able
 */
int store_client_name(struct client_info *client);

/**
 *  Enqueue a msg_packet struct into the client's msg_queue.
 *
 *
 *  @param client   Pointer to client's info struct.
 *
 *  @return         0 on success, -1 on failure
 */
int client_enqueue_msg_packet(struct client_info *client, 
                              struct msg_packet *packet, 
                              size_t msg_size_limit);

#endif
