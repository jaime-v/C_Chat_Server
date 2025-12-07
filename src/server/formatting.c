#include "formatting.h"
#include "client_info.h"
#include "utils.h"
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>

uint8_t *format_chat_message(const struct client_info *client){
  // Create buffer for timestamp and get timestamp
  char timestamp[20];
  get_timestamp(timestamp, sizeof(timestamp));
  size_t timestamp_len = strlen(timestamp);

  // String length is (assuming msg is stripped of \n):
  //     Length of timestamp
  //     Space
  //     Length of name ( -1 for null terminator)
  //     Colon + Space
  //     Length of msg ( -1 for null terminator)
  //     \n + \0
  size_t string_len = timestamp_len + 1 + client->name_len + 2 + client->partial_len + 2;

  // Malloc buffer
  uint8_t *formatted_msg = (uint8_t *)malloc(string_len);
  if(!formatted_msg){
    return NULL;
  }

  // Manual formatting because we want to be able to handle binary data
  size_t index = 0;
  memcpy(formatted_msg + index, timestamp, timestamp_len);
  index += timestamp_len;
  formatted_msg[index++] = ' ';
  memcpy(formatted_msg + index, client->name, client->name_len);
  index += client->name_len;
  formatted_msg[index++] = ':';
  formatted_msg[index++] = ' ';
  memcpy(formatted_msg + index, client->partial_msg, client->partial_len);
  index += client->partial_len;

  // Format message into formatted_msg
  /*
  int chars_written = snprintf(
      (char *)formatted_msg, 
      string_len,
      "%s %.*s: %.*s",
      timestamp,
      (int)client->name_len,
      client->name,
      (int)client->partial_len,
      client->partial_msg
  );
  */

  // Set the end of the message
  formatted_msg[index++] = '\n';
  formatted_msg[index] = '\0'; 


  // Error check for snprintf
  if(index >= string_len){
    free(formatted_msg);
    return NULL;
  }

  return formatted_msg;
}

uint8_t *format_whisper_message(const struct client_info *sender, const uint8_t *msg, size_t msg_len){
  // Create buffer for timestamp and get timestamp
  char timestamp[20];
  get_timestamp(timestamp, sizeof(timestamp));
  size_t timestamp_len = strlen(timestamp);

  char *whisper_thing = " [Whisper from: ";
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
      timestamp_len
      + 2 
      + whisper_thing_len 
      + sender->name_len 
      + 3 
      + msg_len 
      + 2);

  uint8_t *formatted_msg = (uint8_t *)malloc(string_len);
  if(formatted_msg == NULL){
    return NULL;
  }

  // Manual formatting because we want to be able to handle binary data
  size_t index = 0;
  memcpy(formatted_msg + index, timestamp, timestamp_len);
  index += timestamp_len;
  memcpy(formatted_msg + index, whisper_thing, whisper_thing_len);
  index += whisper_thing_len;
  memcpy(formatted_msg + index, sender->name, sender->name_len);
  index += sender->name_len;
  formatted_msg[index++] = ']';
  formatted_msg[index++] = ':';
  formatted_msg[index++] = ' ';
  memcpy(formatted_msg + index, msg, msg_len);
  index += msg_len;

  // Format message into formatted_msg
  /*
  int chars_written = snprintf(
      (char *)formatted_msg, 
      string_len,
      "%s [Whisper from: %.*s]: %.*s",
      timestamp,
      (int)sender->name_len,
      sender->name,
      (int)msg_len,
      msg
  );
  */

  // End of message
  formatted_msg[index++] = '\n';
  formatted_msg[index] = '\0';

  // Error check for snprintf
  if(index >= string_len){
    free(formatted_msg);
    return NULL;
  }

  return formatted_msg;
}
