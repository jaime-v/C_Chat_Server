#include "client_info.h"
#include "client_info_init.h"
#include "common_header.h"
#include <stdlib.h>

int client_info_init(struct client_info *client_out, int client_fd){
  client_out->cfd = client_fd;
  client_out->partial_msg = (char *)malloc(BUF_SIZE);
  if(client_out->partial_msg == NULL){
    return -1;
  }
  client_out->partial_len = 0;
  client_out->partial_cap = BUF_SIZE;
  return 0;
}

