#ifndef BROADCAST_H
#define BROADCAST_H

#include "client_info.h"
#include "server_state.h"

/**
 * Broadcasts msg to all clients except sender.
 *
 * Internally locks and unlocks server state mutex.
 * Create a message header, send the header to client, send payload msg to client.
 * Repeat for all clients.
 *
 * @param state   Global server state
 * @param sender  Pointer to client_info struct for the sender of msg
 * @param msg     Formatted message to send to other chat clients
 * @param len     Length of formatted message
 *
 * @return        0 on success, -1 on failure
 */
int broadcast(struct server_state *state, struct client_info *sender, char *msg, size_t len);

/**
 * Sends a message from server directly to a client's accepted file descriptor.
 *
 * @param cfd     Client's file descriptor, function writes to this
 * @param msg     Formatted message to send to other chat clients
 * @param len     Length of formatted message
 *
 * @return        0 on success, -1 on failure
 */
int server_send_message(int cfd, char *msg, size_t len);

#endif
