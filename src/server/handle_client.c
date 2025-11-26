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
#include "command.h"
#include "server_handle_command.h"
#include "utils.h"
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

void *handle_client(void *args){
  struct client_thread_args *arg = (struct client_thread_args *)args;
  struct server_state *state = arg->state;
  struct client_info *info = arg->info;
  free(arg);

  ssize_t bytes_read;
  bool have_name = false;
  struct msg_header header;

  while((bytes_read = read_header(info->cfd, &header)) > 0){
    size_t msg_len = ntohl((uint32_t)header.msg_len);
    bool msg_done = header.msg_done;
    char *msg_payload = NULL; // Create the null buffer
    printf("[DEBUG - handle_client]: Reading header - msg_len: %zu\n", msg_len);

    if(msg_len == 0){
      printf("[DEBUG - handle_client]: Payload is 0\n");
    } else {
      printf("[DEBUG - handle_client]: Reading payload\n");
      // read_payload mallocs for msg_payload internally
      if((bytes_read = read_payload(info->cfd, &msg_payload, msg_len)) <= 0){
        // Maybe add in a check for msg_len?
        // Anyhow, this shouldn't be possible because if, and only if, payload is > 0, we send a 
        // header
        // Not true anymore, payload can be 0 because we dont include the null terminator anymore
        break;
      }
  
      // printf("[DEBUG - handle_client]: Received payload (should be null terminated now): %s\n", msg_payload);
      printf("[DEBUG - handle_client]: Reading header - msg_len: %zu\n", msg_len);
      printf("[DEBUG - handle_client]: Individual bytes of msg_payload:\n");
      for(size_t i = 0; i < msg_len + 1; i++){
        printf("%zu: %d -- %c\n", i, msg_payload[i], msg_payload[i]);
      }
  
      // Append msg_payload to info->partial_msg and increase info->partial_len
      // append_to_client_buffer frees msg_payload internally
      printf("[DEBUG - handle_client]: Appending %zu bytes to client buffer\n", msg_len);
      if(append_to_client_buffer(info, msg_payload, msg_len) == -1){
        // error here
      }
    }

    if(msg_done){
      // We have entire payload, so make a copy of it
      // printf("We have: %s and %zu\n", info->partial_msg, info->partial_len);

      // Create copy of total payload
      char *payload_copy = (char *)malloc(info->partial_len + 1);
      payload_copy[info->partial_len] = '\0';
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


        printf("[DEBUG - handle_client]:\n\tcmd: %s\n", cmd);
        if(*rest_of_message == '\0'){
          printf("No args\n");
        }
 
        // Make command lowercase -- for case insensitive commands
        size_t cmd_len = strlen(cmd);
        /*
        for(size_t i = 1; i < cmd_len; i++){
          if(cmd[i] >= 'A' && cmd[i] <= 'Z'){
            cmd[i] += ('a' - 'A');
          }
        }
        */
        make_lowercase(cmd, cmd_len);
        // to_lowercase();
  
        // Execute command
        // handle_command();
        enum CMD_RES command_result = server_handle_command(
            state,
            info,
            cmd,
            saveptr
        );
        if(command_result == CMD_DISCONNECT || command_result == CMD_ERROR){
          free(payload_copy);
          remove_client_from_list(state, info);
          return NULL;
        }
        if(command_result == CMD_WARNING){
          printf("Something went wrong MONKA\n");
        }

        /*
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
            shutdown(state->server_fd, SHUT_RDWR);
            if(close(state->server_fd) == -1) {
              printf("[handle_client]: closing error\n");
            } else {
              printf("[handle_client]: close success\n");
            }
            free(payload_copy);
            printf("Returning NULL\n");
            return NULL;
          case CMD_WHISPER:
            printf("Whispering\n");
            if(rest_of_message == NULL){
              printf("Not enough args\n");
              break;
            }

            // Get whisper target's name and message to whisper
            char *whisper_target = strtok_r(NULL, " ", &saveptr);
            char *whisper_msg = saveptr;
            size_t whisper_len = strlen(whisper_msg);
            if(*whisper_msg == '\0'){
              printf("No message for whisper\n");
              break;
            }
            
            // Find whisper target
            struct client_info *target = NULL;
            for(size_t i = 0; i < state->client_count; i++){
              if(strcmp(whisper_target, state->client_list[i]->name) == 0){
                target = state->client_list[i];
                break;
              }
            }
            if(target == NULL){
              printf("Target not found\n");
              break;
            }

            // Format whisper
            // Uses malloc internally
            char *formatted_whisper = format_whisper_message(
                (const struct client_info *)info, 
                (const char *)whisper_msg, 
                whisper_len);
            size_t formatted_whisper_len = strlen(formatted_whisper);

            // Send to target
            if(client_send_direct_message(
                  target->cfd, 
                  formatted_whisper, 
                  formatted_whisper_len) == -1){
              printf("error whispering to client\n");
            }
            free(formatted_whisper);

            break;
          case CMD_LIST:
            printf("Listing\n");
            // Go through the client list, printing sending the names of each client to the sender
            for(size_t i = 0; i < state->client_count; i++){
              char *server_msg = state->client_list[i]->name;
              size_t server_msg_len = state->client_list[i]->name_len;
              server_send_message(info->cfd, server_msg, server_msg_len);

              if(state->client_list[i] == info){
                server_send_message(info->cfd, " <-- YOU\n", 9);
              } else {
                server_send_message(info->cfd, "\n", 1);
              }
            }
            break;
          case CMD_UNKNOWN:
          default:
            printf("Unknown\n");
            break;
        }
      */
      } else {
        printf("[DEBUG - handle_client]: Attempting to broadcast\n");
        // BROADCAST MESSAGE
        char *formatted_msg = format_chat_message(info);
        size_t formatted_len = strlen(formatted_msg);

        // DEBUG
        printf("[DEBUG - handle_client]: Formatted_len: %zu\n", formatted_len);
        printf("[DEBUG - handle_client]: Formatted msg:\n");
        for(size_t i = 0; i < formatted_len; i++){
          printf("%zu: %d -- %c\n", i, formatted_msg[i], formatted_msg[i]);
        }


        if(broadcast(state, info, formatted_msg, formatted_len) == -1){
          // error
        }
        free(formatted_msg);
      }

      // After processing a message, free the copy and reset partial_len
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
