#include "process_payload.h"
#include "client_utils.h"
#include "utils.h"
#include "server_handle_command.h"
#include "broadcast.h"
#include "formatting.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

int process_payload(struct server_state *state, struct client_info *info, uint8_t *payload_copy){
  if(info->has_name == 0){
    printf("STORING NAME\n");
    store_client_name(info);
    info->has_name = 1;
  } else if (payload_copy[0] == '/'){
    printf("COMMAND DETECTED\n");

    // Tokenize
    char *saveptr;
    char *cmd = strtok_r((char *)payload_copy, " ", &saveptr);
    char *rest_of_message = saveptr;

    if(*rest_of_message == '\0'){
      printf("No args\n");
    }
    size_t cmd_len = strlen(cmd);
    make_lowercase(cmd, cmd_len);
    printf("cmd: %s\n", cmd);
    printf("rest: %s\n", rest_of_message);

    enum CMD_RES command_result = server_handle_command(state, info, cmd, saveptr);
    if(command_result == CMD_DISCONNECT || command_result == CMD_ERROR){
      perror("[DEBUG - process_payload]: Disconnect or error");
      return -1;
    }
    if(command_result == CMD_WARNING){
      perror("[DEBUG - process_payload]: Something went wrong MONKA");
    }
  } else {
    printf("BROADCAST DETECTED\n");
    uint8_t *formatted_msg = format_chat_message(info);
    size_t formatted_len = strlen((char *)formatted_msg);
    broadcast(state, info, formatted_msg, formatted_len);
    free(formatted_msg);
  }
  printf("We have a valid payload processed\n");


  return 0;
}
