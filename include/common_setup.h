#ifndef COMMON_SETUP_H
#define COMMON_SETUP_H

#include <sys/socket.h>
#include <netinet/in.h>
#define ERR_SOCKET        -1
#define ERR_INET_PTON     -2

/**
 *  Create socket file descriptor at sfd_out.
 *  Function modifies sfd_out internally.
 *
 *  @param sfd_out    Pointer to int where the file descriptor will be stored. 
 *
 *  @return           0 on success, ERR_SOCKET (-1) on error.
 */
int create_inet_socket(int *sfd_out);

/**
 *  Initializes address pointed to by addr.
 *
 *  @param addr_out   Pointer to address struct.
 *  @param src        String indicating IPv4 address.
 *
 *  @return           0 on success, ERR_INET_PTON (-2) on error.
 */
int setup_address(struct sockaddr_in *addr_out, char *src);

#endif
