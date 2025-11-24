#include "protocol.h"
#include <stdlib.h>
#include <arpa/inet.h>


ssize_t read_header(int fd, struct msg_header *header_out){
  return read_all(fd, header_out, sizeof(struct msg_header));
}

ssize_t read_payload(int fd, char **payload_out, size_t msg_len){
  *payload_out = (char *)malloc(msg_len + 1);
  if(*payload_out == NULL) { return -1; }
  // payload_out[msg_len] = '\0';
  return read_all(fd, *payload_out, msg_len);
}

ssize_t read_packet(int fd, struct msg_header *header_out, char **payload_out){
  // Read in header
  ssize_t bytes_read = read_header(fd, header_out);
  if(bytes_read <= 0) { return bytes_read; }

  // Extract message length from header
  size_t msg_len = ntohl((uint32_t)header_out->msg_len);

  // Allocate buffer for payload
  *payload_out = (char *)malloc(msg_len);
  if(*payload_out == NULL){ return -1; }

  // Read payload and store in payload_out
  bytes_read = read_payload(fd, payload_out, msg_len);
  if(bytes_read <= 0){
    free(*payload_out);
    return bytes_read;
  }

  // Return the amount of bytes read from payload
  return bytes_read;
}

ssize_t write_header(int fd, const struct msg_header *header){
  return write_all(fd, header, sizeof(struct msg_header));
}

ssize_t write_payload(int fd, const char *payload, size_t msg_len){
  return write_all(fd, payload, msg_len);
}

ssize_t write_packet(
    int fd, 
    const struct msg_header *header, 
    const char *payload, 
    size_t msg_len
)
{
    if(write_header(fd, header) == -1) { return -1; }
    return write_payload(fd, payload, msg_len); 
}


int create_header(
    struct msg_header *header_out, 
    size_t msg_len, 
    uint8_t msg_type, 
    bool msg_done
)
{
  if(header_out == NULL) { return -1; }
  header_out->msg_len = htonl((uint32_t)msg_len);
  header_out->msg_type = msg_type;
  header_out->msg_done = msg_done;
  return 0;
}

