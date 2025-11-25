#include "formatting.h"
#include "client_info.h"
#include "utils.h"
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>

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
  size_t string_len = strlen(timestamp) + 1 + client->name_len + 2 + client->partial_len + 2;
  printf("[DEBUG - formatting]: client partial len: %zu\n", client->partial_len);
  printf("[DEBUG - formatting]: client partial len: %zu\n", client->name_len);

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

char *format_whisper_message(const struct client_info *sender, const char *msg, size_t msg_len){
  // Create buffer for timestamp and get timestamp
  char timestamp[20];
  get_timestamp(timestamp, sizeof(timestamp));

  char *whisper_thing = "Whisper from: ";
  size_t whisper_thing_len = strlen(whisper_thing);

  // String length is (assuming msg is stripped of \n):
  //     Length of timestamp
  //     Space
  //     Opening square bracket
  //     Length of "Whisper from: "
  //     Length of sender's name
  //     Closing square bracket
  //     Colon + space
  //     Length of msg 
  //     \n + \0
  size_t string_len = ( 
      strlen(timestamp) 
      + 2 
      + whisper_thing_len 
      + sender->name_len 
      + 3 
      + msg_len 
      + 2);

  char *formatted_msg = (char *)malloc(string_len);

  // Format message into formatted_msg
  int chars_written = snprintf(
      formatted_msg, 
      string_len,
      "%s [Whisper from: %.*s]: %.*s",
      timestamp,
      (int)sender->name_len,
      sender->name,
      (int)msg_len,
      msg
  );

  formatted_msg[string_len - 2] = '\n';
  formatted_msg[string_len - 1] = '\0';


  // Error check for snprintf
  if(chars_written < 0 || chars_written >= (int)string_len){
    free(formatted_msg);
    return NULL;
  }

  return formatted_msg;
}
