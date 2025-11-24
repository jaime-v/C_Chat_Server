#include "common_setup.h"
#include "common_header.h"
#include <string.h>
#include <arpa/inet.h>


// Debug
#include <stdio.h>

int create_inet_socket(int *sfd_out){
  *sfd_out = socket(AF_INET, SOCK_STREAM, 0);
  if(*sfd_out == -1){
    return ERR_SOCKET;
  }
  return 0;
}

int setup_address(struct sockaddr_in *addr_out, char *src){
  memset(addr_out, 0, sizeof(*addr_out));
  addr_out->sin_family = AF_INET;
  addr_out->sin_port = htons(PORT);
  if(inet_pton(AF_INET, src, &addr_out->sin_addr) != 1){
    return ERR_INET_PTON;
  }
  return 0;
}

