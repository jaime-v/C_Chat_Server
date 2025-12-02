#include "handle_client_read.h"
#include "process_payload.h"
#include "utils.h"
#include "client_utils.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>

int handle_client_read(struct server_state *state, struct client_info *info){
  for(;;){
    if(info->state == READ_HEADER){
      printf("\n\nReading header\n");
      printf("Have read %zu bytes so far\n", info->header_bytes_read);
      printf("Expecting %zu bytes\n", sizeof(struct msg_header));
      ssize_t bytes_read = read(info->client_fd,
                                info->header_buffer + info->header_bytes_read,
                                sizeof(struct msg_header) - info->header_bytes_read);
      printf("Read %zu bytes\n", bytes_read);
      if(bytes_read == 0){
        // Shouldn't be happening unless we get a shutdown
        // Assume client closed connection
        // Remove client
        perror("Read 0 bytes from header, sending -1 to server_loop");
        return -1;
      }

      if(bytes_read < 0){
        if(errno == EAGAIN || errno == EWOULDBLOCK){
          // No more data to read
          perror("Got would block error, sending 0 to server_loop");
          return 0;
        } else {
          perror("handle_client_read - read, sending -1 to server_loop");
          return -1;
        }
      }

      info->header_bytes_read += (size_t)bytes_read;

      if(info->header_bytes_read < sizeof(struct msg_header)){
        // Exit early because we need more header bytes
        perror("too little header bytes, returning early, sending 0");
        return 0;
      }

      if(info->header_bytes_read > sizeof(struct msg_header)){
        // Exit early because we have too many header bytes
        perror("too many header bytes, sending -1");
        return -1;
      }

      // If we reach this point, we can parse the header because we have received all the bytes
      struct msg_header *header = (struct msg_header *)info->header_buffer;

      info->expected_payload_len = (size_t)ntohl((uint32_t)header->msg_len);
      info->msg_type = header->msg_type;
      info->msg_done = header->msg_done;

      if(info->expected_payload_len > BUF_SIZE){
        // This shouldnt be possible
        perror("Expected payload is greater than buf size monka");
      }

      // reset for payload
      info->state = READ_PAYLOAD; 
      info->payload_bytes_read = 0;

    } else if (info->state == READ_PAYLOAD){
      printf("\n\nReading payload\n");
      printf("Have read %zu bytes so far\n", info->payload_bytes_read);
      printf("Expecting %zu bytes\n", info->expected_payload_len);
      ssize_t bytes_read = read(info->client_fd,
                                info->payload_buffer + info->payload_bytes_read,
                                info->expected_payload_len - info->payload_bytes_read);
      printf("Read %zu bytes\n", bytes_read);
      if(bytes_read == 0){
        // Possibility of payload with 0 bytes
        // So we probably just leave it
        perror("ZERO payload");
      }

      if(bytes_read < 0){
        if(errno == EAGAIN || errno == EWOULDBLOCK){
          // No more data to read
          perror("We got a would block error in payload, sending 0 to server_loop");
          return 0;
        } else {
          perror("handle_client_read - read, sending -1 to server_loop");
          return -1;
        }
      }

      info->payload_bytes_read += (size_t) bytes_read;

      if(info->payload_bytes_read < info->expected_payload_len){
        // Exit early because we need more payload bytes
        perror("Exiting early becase we need more payload bytes, sending 0");
        return 0;
      }

      if(info->payload_bytes_read > info->expected_payload_len){
        // Exit early because we need more payload bytes
        perror("Too many payload bytes somehow, sending -1");
        return -1;
      }

      // If we reach this point, then our payload should be complete
      // Put the stuff into the client's big buffer
      if(append_to_client_buffer(info) == -1){
        perror("append_to_client_buffer");
      }

      // Check if the msg is done
      if(info->msg_done){
        printf("Message is done\n");
        uint8_t *payload_copy = copy_buffer(info->partial_msg, info->partial_len);

        // Process message
        int payload_result = process_payload(state, info, payload_copy);
        free(payload_copy);
        if(payload_result == -1){
          perror("Disconnect");
          return -1;
        } else {
          printf("payload has been processed: %d\n", payload_result);
        }
      }

      // If it's not done, then we just switch back to reading the next header
      // Reset header reading
      info->state = READ_HEADER;
      info->header_bytes_read = 0;
    } else{
      perror("This should not be possible");
      return -1;
    }
  }
}
