#include "client_info.h"
#include "client_info_init.h"
#include "common_header.h"
#include <stdlib.h>

int client_info_init(struct client_info *client_out, int client_fd){
  // client_out->cfd = client_fd;
  client_out->partial_msg = (uint8_t *)malloc(BUF_SIZE);
  if(client_out->partial_msg == NULL){
    return -1;
  }
  client_out->partial_len = 0;
  client_out->partial_cap = BUF_SIZE;

  client_out->client_fd = client_fd;
  client_out->state = READ_HEADER;

  client_out->header_bytes_read = 0;
  client_out->payload_bytes_read = 0;
  client_out->expected_payload_len = 0;

  client_out->name_len = 0;
  client_out->has_name = 0;

  return 0;
}

