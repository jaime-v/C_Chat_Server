#include "common_setup.h"
#include "client_setup.h"

int init_client(int *sfd, struct sockaddr_in *addr){
  int status;
  if((status = create_inet_socket(sfd)) != 0){
    return status;
  }

  if((status = setup_address(addr, ADDR)) != 0){
    return status;
  }

  if(connect(*sfd, (struct sockaddr *)addr, sizeof(*addr)) == -1){
    return ERR_CONNECT;
  }

  return status;
}
