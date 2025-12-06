#include "common_header.h"
#include "client_utils.h"
#include <stdlib.h>
#include <string.h>

// DEBUG
#include <stdio.h>

int append_to_client_buffer(struct client_info *client){
  // If message len + bytes read from payload > message cap, expand message cap
  if(client->partial_len + client->payload_bytes_read > client->partial_cap){
    printf("We are here\n");
    while(client->partial_cap < client->partial_len + client->payload_bytes_read) { 
      printf("We are doubling\n");
      client->partial_cap *= 2; 
    }
    printf("We are calling realloc\n");
    client->partial_msg = realloc(client->partial_msg, client->partial_cap);
    if(client->partial_msg == NULL){
      printf("We have a realloc error\n");
      return -1;
    }
  }

  // Copy message into client struct
  memcpy(client->partial_msg + client->partial_len, 
         client->payload_buffer, 
         client->payload_bytes_read);
  client->partial_len += client->payload_bytes_read;
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


int client_enqueue_msg_packet(struct client_info *client, 
                              struct msg_packet *packet, 
                              size_t msg_size_limit){
  if(packet == NULL){
    return -1;
  }

  if(client->msg_queue.queued_bytes + packet->len > msg_size_limit){
    // Can't queue up more messages
    return -1;
  }

  if(client->msg_queue.tail != NULL){
    // If queue is nonempty, make the current tail element point to the packet.
    // Then make the packet the new tail element.
    client->msg_queue.tail->next = packet;
    client->msg_queue.tail = packet;
  } else {
    // Otherwise, the queue is empty, so head and tail can point to the same packet.
    client->msg_queue.head = packet;
    client->msg_queue.tail = packet;
  }

  // Increase the size of the msg_queue and number of queued messages
  client->msg_queue.queued_bytes += packet->len;
  client->msg_queue.queued_count++;

  printf("[DEBUG - client_utils]: enqueue packet to client: %d\n", client->client_fd);
  return 0;

}
