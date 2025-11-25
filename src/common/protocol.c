#include "protocol.h"
#include <stdlib.h>
#include <arpa/inet.h>


ssize_t read_header(int fd, struct msg_header *header_out){
  return read_all(fd, header_out, sizeof(struct msg_header));
}

ssize_t read_payload(int fd, char **payload_out, size_t msg_len){
  // Malloc out the payload string (should be msg_len + 1 for null)
  *payload_out = (char *)malloc(msg_len + 1);
  if(*payload_out == NULL) { return -1; }
  // msg_len = 5, msg = Hello
  // payload_out should be Hello\0
  (*payload_out)[msg_len] = '\0';
  // Return the number of bytes read into the buffer, with a maximum of msg_len bytes
  return read_all(fd, *payload_out, msg_len);

  /* Potentially better version (More verbose, less working with double pointers)
   *
   * char *buf = malloc(msg_len) + 1;
   * if(buf == NULL){ return -1; }
   * ssize_t bytes_read = read_all(fd, buf, msg_len);
   * if(bytes_read != msg_len){
   *    free(buf);
   *    return -1;
   * }
   * buf[msg_len] = '\0';
   * *payload_out = buf;
   * return bytes_read;
   *
   */
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

