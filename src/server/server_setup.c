#include "common_setup.h"
#include "server_setup.h"

#include <fcntl.h>

int init_server(int *sfd, struct sockaddr_in *addr){
  int status;
  
  status = create_inet_socket(sfd);
  if(status != 0) { return status; }

  // EPOLL Stuff - set server file descriptor to non-blocking
  status = fcntl(*sfd, F_SETFL, O_NONBLOCK);
  if(status == -1){ return status; }

  status = setup_address(addr, LISTEN_ADDR);
  if(status != 0) { return status; }

  status = bind_and_listen(sfd, addr);
  if(status != 0) { return status; }

  return status;
}

int bind_and_listen(int *sfd, struct sockaddr_in *addr){
  int opt = 1;
  if(setsockopt(*sfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1){
    return ERR_SOCKET;
  }

  if(bind(*sfd, (struct sockaddr *)addr, sizeof(*addr)) == -1){
    return ERR_BIND;
  }

  if(listen(*sfd, LISTEN_BACKLOG) == -1){
    return ERR_LISTEN;
  }
  return 0;
}
