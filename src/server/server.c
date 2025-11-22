#include "server_setup.h"
#include "server_state.h"
#include "server_loop.h"
#include "logging.h"

#include <stdio.h>
#include <netinet/in.h>

int main(void){
  // int sfd;
  struct sockaddr_in addr;
  struct server_state state;

  // Init server state
  if(server_state_init(&state) == -1){
    handle_error("server_state_init");
  }
  printf("[server.c] AFTER INIT SERVER FD -- Server state: %p\n", (void *)&state);
  printf("[server.c] AFTER INIT SERVER FD -- Server fd: %p = %d\n", (void *)&state.server_fd, state.server_fd);

  // Init server socket -- pass a pointer to state.server_fd (pointer to int)
  if(init_server(&state.server_fd, &addr) == -1){
    handle_error("init_server");
  }
  printf("[server.c] AFTER INIT SOCKET FD: %p\n", (void *)&state);
  printf("[server.c] AFTER INIT SOCKET FD: %p = %d\n", (void *)&state.server_fd, state.server_fd);

  /*
  // Server loop logic
  if(server_loop(&state, sfd) == -1){
    fprintf(stderr, "SERVER EXITED WITH ERROR\n");
  }
  */

  // Server loop logic
  if(server_loop(&state) == -1){
    fprintf(stderr, "SERVER EXITED WITH ERROR\n");
  }

  // Clean up server state
  if(server_state_destroy(&state) == -1){
    handle_error("server_state_destroy");
  }
  return 0;
}
