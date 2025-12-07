#ifndef MSG_PACKET_H
#define MSG_PACKET_H

#include "protocol.h"
#include <stdint.h>
#include <stddef.h>

struct msg_packet{
  struct msg_packet *next;
  uint8_t *packet_data;
  size_t len;
  size_t offset;
};

struct msg_packet *create_packet(struct msg_header *header, uint8_t *payload, size_t payload_len);

#endif
