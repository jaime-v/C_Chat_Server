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
    if(store_client_name(info) == -1){
      perror("store_client_name");
    }
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

    enum CMD_RES command_result = server_handle_command(state, info, cmd, saveptr);
    if(command_result == CMD_DISCONNECT || command_result == CMD_ERROR){
      perror("Disconnect or error");
      return -1;
    }
    if(command_result == CMD_WARNING){
      perror("Something went wrong MONKA");
    }
  } else {
    printf("ONLY OTHER COMMAND IS BROADCAST\n");
    char *formatted_msg = format_chat_message(info);
    size_t formatted_len = strlen(formatted_msg);
    if(broadcast(state, info, formatted_msg, formatted_len) == -1){
      perror("Broadcasting error");
    }
    free(formatted_msg);
  }


  return 0;
}
