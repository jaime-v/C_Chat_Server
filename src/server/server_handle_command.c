#include "server_handle_command.h"
#include "server_control.h"
#include "broadcast.h"
#include "formatting.h"
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>


enum CMD_RES server_handle_command(
    struct server_state *state, 
    struct client_info *user, 
    char *cmd,
    char *saveptr
  ){
  enum CMD command = find_command(cmd);
  printf("[DEBUG - server_handle_command]: Command: %d\n", command);
  switch(command){
    case CMD_LIST:
      return handle_list(state, user);
    case CMD_WHISPER:
      return handle_whisper(state, user, saveptr);
    case CMD_SHUTDOWN:
      if(handle_shutdown() == CMD_WARNING){
        printf("Had a warning in shutdown\n");
      }
      return CMD_DISCONNECT;
    case CMD_UNKNOWN:
    default:
      return server_handle_unknown();
  }
  return CMD_WARNING;
}

enum CMD_RES handle_list(struct server_state *state, struct client_info *user){
  printf("Listing\n");
  // Go through the client list, printing sending the names of each client to the sender
  for(size_t i = 0; i < state->client_count; i++){
    // Get client's name and name_len from the list
    uint8_t *server_msg = state->client_list[i]->name;
    size_t server_msg_len = state->client_list[i]->name_len;

    // Extra indicator for indicating the client using this command
    char *indicator = " <-- YOU\n";
    size_t indicator_len = strlen(indicator);

    // Send the name of each client
    server_send_message(user->client_fd, server_msg, server_msg_len);

    // If the client using the command is the same client we see in the list
    if(state->client_list[i] == user){
      // Indicate that the user is this client
      server_send_message(user->client_fd, (uint8_t *)indicator, indicator_len);
    } else {
      // Otherwise, just make a newline for the next client
      server_send_message(user->client_fd, (uint8_t *)"\n", 1);
    }
  }
  return CMD_OK;
}

enum CMD_RES handle_whisper(
    struct server_state *state, 
    struct client_info *user, 
    char *saveptr
  ){
  printf("Whispering\n");
  // Check if we have no whisper target
  if(saveptr == NULL){
    printf("Not enough args\n");
    return CMD_WARNING;
  }

  // Tokenize whisper target's name and message to whisper
  char *whisper_target = strtok_r(NULL, " ", &saveptr);
  char *whisper_msg = saveptr;
  size_t whisper_len = strlen(whisper_msg);

  // Check if we have not whisper message
  if(*whisper_msg == '\0'){
    printf("No message for whisper\n");
    return CMD_WARNING;
  }
            
  // Find whisper target
  struct client_info *target = NULL;
  for(size_t i = 0; i < state->client_count; i++){
    // NOTE: 
    // For case insensitivity, we can copy the buffer and make it lowercase
    // we have functions for both of those
    if(strcmp(whisper_target, (char *)state->client_list[i]->name) == 0){
      target = state->client_list[i];
      break;
    }
  }
  // Check if target is not in the client list
  if(target == NULL){
    printf("Target not found\n");
    return CMD_WARNING;
  }

  // Format whisper
  // Uses malloc internally
  uint8_t *formatted_whisper = format_whisper_message(
      (const struct client_info *)user, 
      (const uint8_t *)whisper_msg, 
      whisper_len);
  
  if(formatted_whisper == NULL){
    return CMD_ERROR;
  }
  size_t formatted_whisper_len = strlen((char *)formatted_whisper);

  // Send to target
  if(client_send_direct_message(
        target->client_fd, 
        formatted_whisper, 
        formatted_whisper_len) == -1){
    printf("error whispering to client\n");
    return CMD_ERROR;
  }

  // Free formatted whisper
  free(formatted_whisper);

  return CMD_OK;
}

enum CMD_RES handle_shutdown(){
  printf("Shutting down\n");
  server_shutdown = 1;
  return CMD_DISCONNECT;
}

enum CMD_RES server_handle_unknown(){
  printf("Unknown Command\n");
  return CMD_OK;
}

