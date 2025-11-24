#include "formatting.h"
#include "client_info.h"
#include "utils.h"
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>

int store_client_name(struct client_info *client){
  // Set partial_len to NAME_MAX if it exceeds NAME_MAX (cap length to be NAME_MAX - 1)
  //    Since we don't include '\0' in the length
  if(client->partial_len >= NAME_MAX){
    client->partial_len = NAME_MAX - 1;
  }

  // Copy into name field
  memcpy(client->name, client->partial_msg, client->partial_len);

  // Set name length and last byte to be null
  client->name_len = client->partial_len;
  client->name[client->name_len] = '\0';
  return 0;
}

char *format_chat_message(const struct client_info *client){
  // Create buffer for timestamp and get timestamp
  char timestamp[20];
  get_timestamp(timestamp, sizeof(timestamp));

  // String length is (assuming msg is stripped of \n):
  //     Length of timestamp
  //     Space
  //     Length of name ( -1 for null terminator)
  //     Colon + Space
  //     Length of msg ( -1 for null terminator)
  //     \n + \0
  size_t string_len = strlen(timestamp) + 1 + client->name_len - 1 + 2 + client->partial_len - 1 + 2;
  printf("[DEBUG - formatting]: client partial len: %zu\n", client->partial_len);

  // Malloc buffer
  char *formatted_msg = (char *)malloc(string_len);
  if(!formatted_msg){
    return NULL;
  }

  // Format message into formatted_msg
  int chars_written = snprintf(
      formatted_msg, 
      string_len,
      "%s %.*s: %.*s",
      timestamp,
      (int)client->name_len,
      client->name,
      (int)client->partial_len,
      client->partial_msg
  );

  // Set the end of the message
  printf("[DEBUG - format_chat_message]: space allocated: %zu\n", string_len); 
  printf("[DEBUG - format_chat_message]: chars written: %d\n", chars_written);
  printf("[DEBUG - format_chat_message]: formatted message: %s\n", formatted_msg);
  for(size_t i = 0; i < string_len; i++){
    printf("%zu: %d -- %c\n", i, formatted_msg[i], formatted_msg[i]);
  }
  formatted_msg[string_len - 2] = '\n';
  formatted_msg[string_len - 1] = '\0';
  printf("[DEBUG - format_chat_message]: altered message: %s\n", formatted_msg);
  for(size_t i = 0; i < string_len; i++){
    printf("%zu: %d -- %c\n", i, formatted_msg[i], formatted_msg[i]);
  }


  // Error check for snprintf
  if(chars_written < 0 || chars_written >= (int)string_len){
    free(formatted_msg);
    return NULL;
  }

  return formatted_msg;
}
