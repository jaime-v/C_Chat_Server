#ifndef CLIENT_LIST_H
#define CLIENT_LIST_H

#include "client_info.h"
#include "server_state.h"

/**
 * Adds a new client to server state's global client list.
 *
 * @param state   Pointer to global server state.
 * @param client  Pointer to client info that will be added to state's client list.
 *
 * @return        0 on success, -1 on failure.
 */
int add_client_to_list(struct server_state *state, struct client_info *client);



/**
 * Removes a given client from server state's global client list.
 * Uses cleanup_client helper function internally.
 *
 * @param state   Pointer to global server state.
 * @param client  Pointer to client info that will be removed from state's client list.
 *
 * @return        0 on success, -1 on failure.
 */
int remove_client_from_list(struct server_state *state, struct client_info *client);



/**
 * Cleans up given client by closing file descriptor and freeing memory for client.
 *
 * @param client  Pointer to client info that will be cleaned up.
 *
 * @return        0 on success, -1 on failure.
 */
int cleanup_client(struct client_info *client);


/**
 * Removes all clients from the server state. Should be used when shutting down.
 *
 * @param state   Pointer to server state struct that is shutting down.
 *
 * @return        0 on success, -1 on failure.
 */
int remove_all_clients(struct server_state *state);
#endif
