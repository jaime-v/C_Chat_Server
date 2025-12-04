#ifndef SERVER_SETUP_H
#define SERVER_SETUP_H

#include <sys/socket.h>
#include <netinet/in.h>
#define ERR_BIND    -3
#define ERR_LISTEN  -4

#define LISTEN_BACKLOG 1024
#define LISTEN_ADDR "0.0.0.0"

/**
 *  Initializes server using file descriptor sfd. Binds and listens on address pointer to by addr.
 *
 *  @param sfd    Server's file descriptor.
 *  @param addr   Address to bind server to.
 *
 *  @return       0 on success, nonzero on failure.
 */
int init_server(int *sfd, struct sockaddr_in *addr);


/**
 *  Helper function that binds sfd to addr and listens on this address.
 *
 *  @param sfd    Server's file descriptor.
 *  @param addr   Address to bind server to.
 *
 *  @return       0 on success, nonzero on failure.
 */
int bind_and_listen(int *sfd, struct sockaddr_in *addr);

#endif
