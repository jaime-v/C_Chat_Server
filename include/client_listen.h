#ifndef CLIENT_LISTEN_H
#define CLIENT_LISTEN_H

struct listen_info {
  int sfd;
};

/**
 * Thread worker function that reads from client socket and prints to stdout.
 *
 * @param args    Pointer to args. Expects a pointer to struct listen_info.
 *
 * @return        NULL. We don't care about return value in this worker function.
 */
void *client_listen(void *args);

#endif
