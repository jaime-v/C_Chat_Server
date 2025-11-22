#ifndef SERVER_LOOP_H
#define SERVER_LOOP_H

#include "server_state.h"

/**
 *  Server's internal loop. Handles accepting clients and spawning client handling threads.
 *
 *  @param state    Pointer to server's state struct.
 *  @param sfd      Server's file descriptor (Can actually remove this since state->sfd has it)
 *
 *  @return         0 on success, -1 if any error occurs.
 */
int server_loop(struct server_state *state);
// int server_loop(struct server_state *state, int sfd);

#endif
