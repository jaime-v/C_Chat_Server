#include "client_handle_command.h"
#include <sys/socket.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

enum CMD_RES client_handle_command(int sfd, char *cmd){
  enum CMD command = find_command(cmd);
  switch(command){
    case CMD_QUIT:
      return handle_quit(sfd);
    case CMD_SHUTDOWN:
      return CMD_DISCONNECT;
    case CMD_UNKNOWN:
    default:
      return client_handle_unknown();
  }
  return CMD_WARNING; // if this is possible then something went very wrong
}

enum CMD_RES handle_quit(int sfd /* , struct listen_info *info */){
  printf("[DEBUG - client_handle_command]: client quitting\n");
  shutdown(sfd, SHUT_RDWR);
  // We will free info in the client driver
  // free(info);
  // info = NULL;
  if(close(sfd) == -1){
    return CMD_WARNING;
  }
  return CMD_DISCONNECT;
}

enum CMD_RES client_handle_unknown(){
  printf("[DEBUG - client_handle_command]: unknown command\n");
  return CMD_OK;
}
