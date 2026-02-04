#include "handle_client_read.h"
#include "client_utils.h"
#include "common_header.h"
#include "process_payload.h"
#include "utils.h"
#include <arpa/inet.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

int handle_client_read(struct server_state *state, struct client_info *info) {
  for (;;) {
    if (info->state == READ_HEADER) {
      ssize_t bytes_read =
          read(info->client_fd, info->header_buffer + info->header_bytes_read,
               sizeof(struct msg_header) - info->header_bytes_read);
      if (bytes_read == 0) {
        // Shouldn't be happening unless we get a shutdown
        // Assume client closed connection
        // Remove client
        perror("handle_client_read - Read 0 bytes from header");
        return -1;
      }

      if (bytes_read < 0) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
          // No more data to read
          // perror("handle_client_read - Got would block error");
          return 0;
        } else {
          perror("handle_client_read - actual read error");
          return -1;
        }
      }

      info->header_bytes_read += (size_t)bytes_read;

      if (info->header_bytes_read < sizeof(struct msg_header)) {
        // Exit early because we need more header bytes
        printf("too little header bytes, returning early\n");
        return 0;
      }

      if (info->header_bytes_read > sizeof(struct msg_header)) {
        // Exit early because we have too many header bytes
        perror("too many header bytes somehow");
        return -1;
      }

      // If we reach this point, we can parse the header because we have
      // received all the bytes
      struct msg_header *header = (struct msg_header *)info->header_buffer;

      info->expected_payload_len = ntohl(header->msg_len);
      info->msg_type = header->msg_type;
      info->msg_done = header->msg_done;

      if (info->expected_payload_len > BUF_SIZE) {
        printf("handle_client_read - expected payload for client %d is too "
               "large\n",
               info->client_fd);
        return -1;
      }

      /*
      if(info->expected_payload_len > BUF_SIZE){
        // This shouldnt be possible
        perror("Expected payload is greater than buf size monka");
        return -1;
      }
      */

      // reset for payload
      info->state = READ_PAYLOAD;
      info->header_bytes_read = 0;
      info->payload_bytes_read = 0;
    } else if (info->state == READ_PAYLOAD) {
      ssize_t bytes_read =
          read(info->client_fd, info->payload_buffer + info->payload_bytes_read,
               info->expected_payload_len - info->payload_bytes_read);

      printf("[INFO: handle_client_read]: info->expected_payload_len: %u\n",
             info->expected_payload_len);
      printf("[INFO: handle_client_read]: bytes_read: %zu\n", bytes_read);
      if (bytes_read == 0) {
        // Possibility of payload with 0 bytes
        // So we probably just leave it
        perror("ZERO payload");
      }

      if (bytes_read < 0) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
          // No more data to read
          // perror("handle_client_read - got a would block error in payload");
          return 0;
        } else {
          perror("handle_client_read - read payload error");
          return -1;
        }
      }

      info->payload_bytes_read += (size_t)bytes_read;

      // DEBUG
      // printf("[DEBUG]: We are expecting: %zu bytes\n",
      //        (size_t)info->expected_payload_len);
      // printf("[DEBUG]: We have: %zu bytes\n", info->payload_bytes_read);

      if (info->payload_bytes_read < info->expected_payload_len) {
        // Exit early because we need more payload bytes
        perror("[INFO]: Exiting early becase we need more payload bytes");
        return 0;
      }

      if (info->payload_bytes_read > info->expected_payload_len) {
        // Exit early because we have too many payload bytes
        perror("[ERROR]: Too many payload bytes somehow");
        return -1;
      }

      // If we reach this point, then our payload should be complete
      // Put the stuff into the client's big buffer
      if (append_to_client_buffer(info) == -1) {
        perror("[WARNING]: append_to_client_buffer");
      }
      // Check if the msg is done
      if (info->msg_done) {
        printf("[DEBUG - handle_client_read]: info->partial_len: %zu\n",
               info->partial_len);
        uint8_t *payload_copy =
            copy_buffer(info->partial_msg, info->partial_len);
        // Process message
        int payload_result = process_payload(state, info, payload_copy);
        free(payload_copy);
        if (payload_result == -1) {
          perror("handle_client_read - Disconnect - process_payload returned "
                 "error");
          return -1;
        }
        // Reset partial_len to 0 so we can overwrite the buffer for next
        // message
        if (clear_client_buffer(info) == -1) {
          perror("handle_client_read - error clearing buffer");
        }
      }

      // If it's not done, then we just switch back to reading the next header
      // Reset header reading
      info->state = READ_HEADER;
      info->header_bytes_read = 0;
      info->expected_payload_len = 0;
    } else {
      // Somehow not reading header or payload
      perror("This should not be possible");
      return -1;
    }
  }
}
