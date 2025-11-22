#include "handle_client.h"
#include "client_info.h"
#include "server_state.h"
#include "protocol.h"
#include "broadcast.h"
#include "formatting.h"
#include "client_thread_args.h"
#include "client_list.h"
#include "client_utils.h"
#include "server_control.h"
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

enum CMD {
  CMD_QUIT,
  CMD_LIST,
  CMD_WHISPER,
  CMD_SHUTDOWN,
  CMD_UNKNOWN
};

struct command_entry {
  const char *name;
  enum CMD cmd;
};

static struct command_entry command_table[] = {
  { "/quit", CMD_QUIT }, 
  { "/list", CMD_LIST },
  { "/whisper", CMD_WHISPER },
  { "/shutdown", CMD_SHUTDOWN },
  { NULL, CMD_UNKNOWN }
};

enum CMD find_command(const char *cmd){
  for (int i = 0; command_table[i].name != NULL; i++){
    if(strcmp(command_table[i].name, cmd) == 0){
      return command_table[i].cmd;
    }
  }
  return CMD_UNKNOWN;
}

void *handle_client(void *args){
  struct client_thread_args *arg = (struct client_thread_args *)args;
  struct server_state *state = arg->state;
  printf("[handle_client] new client: state: %p\n", (void *)state);
  printf("[handle_client] new client: state->server_fd: %p = %d\n", (void *)&state->server_fd, 
      state->server_fd);
  struct client_info *info = arg->info;
  printf("[handle_client] new client: info's cfd: %p = %d\n", (void *)&info->cfd, info->cfd);
  free(arg);

  ssize_t bytes_read;
  bool have_name = false;
  struct msg_header header;

  while((bytes_read = read_header(info->cfd, &header)) > 0){
    size_t msg_len = ntohl((uint32_t)header.msg_len);
    bool msg_done = header.msg_done;
    char *msg_payload = NULL;

    // Debug
    // printf("Received header with len: %zu\n", msg_len);

    if((bytes_read = read_payload(info->cfd, &msg_payload, msg_len)) <= 0){
      // Maybe add in a check for msg_len?
      // Anyhow, this shouldn't be possible because if, and only if, payload is > 0, we send a 
      // header
      break;
    }

    // Debug
    // printf("Received payload\n");
    // Can use this one if we decide to send null terminated, or set up properly
    // printf("Received payload: %s\n", msg_payload);


    // Append msg_payload to info->partial_msg and increase info->partial_len
    if(append_to_client_buffer(info, msg_payload, msg_len) == -1){
      // error here
    }

    if(msg_done){
      // We have entire payload, so make a copy of it
      // printf("We have: %s and %zu\n", info->partial_msg, info->partial_len);

      // Create copy of total payload
      char *payload_copy = (char *)malloc(info->partial_len);
      memcpy(payload_copy, info->partial_msg, info->partial_len);

      // Control Flow processing the message
      if(!have_name){
        // STORE NAME
        if(store_client_name(info) == -1){
          // error
        }
        have_name = true;
      } else if (payload_copy[0] == '/'){
        // RUN COMMAND
        printf("COMMAND\n");
 
        // Tokenize command from copy
        char *saveptr;
        char *cmd = strtok_r(payload_copy, " ", &saveptr);
        char *rest_of_message = saveptr;
 
        // Make command lowercase -- for case insensitive commands
        size_t cmd_len = strlen(cmd);
        for(size_t i = 1; i < cmd_len; i++){
          if(cmd[i] >= 'A' && cmd[i] <= 'Z'){
            cmd[i] += ('a' - 'A');
          }
        }
  
        // Currently, we have the command and the rest of the message
        printf("%s\n", cmd);
        printf("%s\n", rest_of_message);

        // Execute command
        enum CMD command = find_command(cmd);
        printf("Command: %d\n", command);
        switch(command){
          case CMD_QUIT:
            printf("Quitting\n");
            remove_client_from_list(state, info);
            free(payload_copy);
            return NULL;
          case CMD_SHUTDOWN:
            printf("Shutting down\n");
            server_shutdown = 1;
            printf("[handle_client]: SERVER SHUTDOWN: %p = %d\n", (void *)&server_shutdown, 
                server_shutdown);
            printf("[handle_client]: Shutting down: state: %p\n", (void *)state);
            printf("[handle_client]: Shutting down: fd: %p = %d\n", (void *)&state->server_fd,
                state->server_fd);
            shutdown(state->server_fd, SHUT_RDWR);
            if(close(state->server_fd) == -1){
              printf("[handle_client] closing error\n");
            } else {
              printf("[handle_client]: close success\n");
            }
            free(payload_copy);
            printf("Returning NULL\n");
            return NULL;
          case CMD_WHISPER:
            printf("Whispering\n");
            break;
          case CMD_LIST:
            printf("Listing\n");
            break;
          case CMD_UNKNOWN:
          default:
            printf("Unknown\n");
            break;
        }
      } else {
        // BROADCAST MESSAGE
        char *formatted_msg = format_chat_message(info);
        size_t formatted_len = strlen(formatted_msg);
        if(broadcast(state, info, formatted_msg, formatted_len) == -1){
          // error
        }
        free(formatted_msg);
      }

      // FREE COPY
      free(payload_copy);
      info->partial_len = 0;
    }
  }
  if(bytes_read == 0){
    printf("read in EOF\n");
  }
  if(bytes_read == -1){
    // error
    // log -> cleanup -> return NULL
  }
  // out of loop
  remove_client_from_list(state, info);
  return NULL;
}
