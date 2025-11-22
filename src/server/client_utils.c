#include "common_header.h"
#include "client_utils.h"
#include <stdlib.h>
#include <string.h>

int append_to_client_buffer(struct client_info *client, char *payload, size_t bytes_read){
  // If message len + bytes read from payload > message cap, expand message cap
  if(client->partial_len + bytes_read > client->partial_cap){
    client->partial_msg = realloc(client->partial_msg, client->partial_cap + BUF_SIZE);
    if(client->partial_msg == NULL){
      return -1;
    }
    client->partial_cap += BUF_SIZE;
  }

  // Copy message into client struct
  memcpy(client->partial_msg + client->partial_len, payload, bytes_read);
  client->partial_len += bytes_read;

  // Free payload when we are done copying
  free(payload);
  return 0;
}

int clear_client_buffer(struct client_info *client){
  memset(client->partial_msg, 0, client->partial_len);
  if(client->partial_msg == NULL){
    return -1;
  }
  client->partial_len = 0;
  client->partial_cap = BUF_SIZE;
  return 0;
}
