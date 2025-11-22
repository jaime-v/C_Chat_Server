#ifndef CLIENT_SETUP_H
#define CLIENT_SETUP_H

#include <sys/socket.h>
#include <netinet/in.h>
#define ADDR "127.0.0.1"
#define ERR_CONNECT -5

/**
 * Initializes client socket with file descriptor sfd and address addr.
 *
 * @param sfd   Pointer to file descriptor to use.
 * @param addr  Pointer to address to use.
 *
 * @return      Status. 
 *              0 on success, 
 *              ERR_SOCKET (-1) on socket setup failure, 
 *              ERR_INET_PTON (-2) on address setup failure.
 *              ERR_CONNECT (-5) on connection failure.
 */
int init_client(int *sfd, struct sockaddr_in *addr);

#endif
