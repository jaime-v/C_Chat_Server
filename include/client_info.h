#ifndef CLIENT_INFO_H
#define CLIENT_INFO_H

#include <sys/types.h>
#include <sys/socket.h>
#include <pthread.h>
#include <stdint.h>
#include "common_header.h"
#include "protocol.h"
#include "msg_queue.h"

#define NAME_MAX 64

enum read_state {
  READ_HEADER,
  READ_PAYLOAD
};

struct client_info {
  // Client's socket
  int client_fd;

  // Read state - reading a header or a payload
  enum read_state state;

  // Fixed buffer because header is a fixed size
  uint8_t header_buffer[sizeof(struct msg_header)];
  // How many bytes we've accumulated into header_buffer
  size_t header_bytes_read;

  // Extracted details from header
  uint32_t expected_payload_len;
  uint8_t msg_type;
  uint8_t msg_done;
  // After reading the header fully, we parse the header into the client_info fields

  // Buffer to receive payload
  uint8_t payload_buffer[BUF_SIZE];
  // How many bytes we've accumulated into payload_buffer
  size_t payload_bytes_read;

  // For writing stuff
  uint8_t *write_buffer;
  size_t write_len;
  size_t write_offset;

  // Message queue for writing and epollout event
  struct msg_queue msg_queue;
  uint8_t epollout_enabled;

  // Name and name_len for user
  uint8_t name[NAME_MAX];
  size_t name_len;
  
  // Flag for having name
  uint8_t has_name;

  // Dynamic array to build up the message
  uint8_t *partial_msg;
  // Current length of dynamic array
  size_t partial_len;
  // Cap of dynamic array
  size_t partial_cap;

  uint8_t closed;

  /*
  pthread_t thread;
  */
};

#endif
