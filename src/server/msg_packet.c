#include "msg_packet.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

struct msg_packet *create_packet(struct msg_header *header, uint8_t *payload, size_t payload_len){
  // Calculate the total packet data size (header + payload)
  size_t total = sizeof(struct msg_header) + payload_len;

  // Malloc out the packet data
  uint8_t *packet_data = (uint8_t *)malloc(total);
  if(packet_data == NULL){
    printf("msg_packet - Error making packet data\n");
    return NULL;
  }

  // Copy header and payload into the packet data
  memcpy(packet_data, header, sizeof(struct msg_header));
  memcpy(packet_data + sizeof(struct msg_header), payload, payload_len);

  // Create the packet structure
  struct msg_packet *packet = malloc(sizeof(*packet));
  if(packet == NULL){
    perror("msg_packet - Error making packet\n");
    free(packet_data);
    return NULL;
  }

  // Set packet fields
  packet->next = NULL;
  packet->packet_data = packet_data;
  packet->len = total;
  packet->offset = 0;
  return packet;
}
