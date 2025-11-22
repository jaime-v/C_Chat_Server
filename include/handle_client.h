#ifndef HANDLE_CLIENT_H
#define HANDLE_CLIENT_H

/**
 *  Thread worker function that handles client processing on the server.
 *
 *  @param args   Pointer to args. Expects a pointer to struct client_thread_args.
 *
 *  @return       NULL. We don't care about return value in this worker function.
 */
void *handle_client(void *args);

#endif
