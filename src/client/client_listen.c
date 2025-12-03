#include "client_listen.h"
#include "protocol.h"
#include "client_control.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <string.h>
#include <fcntl.h>

void *client_listen(void *args){
  struct listen_info *info = (struct listen_info *)args;
  ssize_t bytes_read;
  struct msg_header header;
  char *msg_payload = NULL;

  while((bytes_read = read_header(info->sfd, &header)) > 0){
    // printf("[DEBUG - client_listen]: Received header\n");
    size_t msg_len = ntohl((uint32_t)header.msg_len);
    if((bytes_read = read_payload(info->sfd, &msg_payload, msg_len)) <= 0){
      // Error
      printf("Couldnt read payload\n");
      free(msg_payload);
      free(info);
      break;
    }

    // printf("[DEBUG - client_listen]: Received payload\n");
    // msg_payload[msg_len - 1] = '\0';
    // printf("[DEBUG - client_listen]: Altered payload: %s\n", msg_payload);
    if(strcmp(msg_payload, "SERVER SHUTDOWN") == 0){
      printf("[client_listen] Shutting down\n");
      client_shutdown = 1;
      free(msg_payload);
      free(info);
      break;
    }

    if(write_payload(STDOUT_FILENO, (const char *)msg_payload, (size_t)bytes_read) == -1){
      // Error
      printf("Couldnt write payload\n");
      free(msg_payload);
      free(info);
      client_shutdown = 1;
      break;
    }
    free(msg_payload);
  }

  if(bytes_read == 0){
    printf("Read in EOF\n");
  }

  if(bytes_read == -1){
    // Error
  }

  printf("\n\n[client_listen] End of client_listen\n\n");
  return NULL;
}
