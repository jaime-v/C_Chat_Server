#include "handle_client_read.h"
#include "process_payload.h"
#include "utils.h"
#include "client_utils.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

int handle_client_read(struct server_state *state, struct client_info *info){
  if(!state || !info){
    return -1;
  }
  return 0;
  /*
  // Loop because ?? idk
  for(;;){
    if(info->state == READ_HEADER){
      ssize_t bytes_read = read(info->client_fd,
                                info->header_buffer + info->header_bytes_read,
                                sizeof(*info) - info->header_bytes_read);
      if(bytes_read == 0){
        // Shouldn't be happening unless we get a shutdown
        // Assume client closed connection
        // Remove client
        return -1;
      }

      if(bytes_read < 0){
        if(errno == EAGAIN || errno == EWOULDBLOCK){
          // No more data to read
          return 0;
        } else {
          perror("handle_client_read - read");
          return -1;
        }
      }

      info->header_bytes_read += (size_t)bytes_read;

      if(info->header_bytes_read < sizeof(*info)){
        // Exit early because we need more header bytes
        return 0;
      }

      // If we reach this point, we can parse the header because we have received all the bytes
      struct msg_header *header = (struct msg_header *)info->header_buffer;

      info->expected_payload_len = header->msg_len;
      info->msg_type = header->msg_type;
      info->msg_done = header->msg_done;

      if(info->expected_payload_len > BUF_SIZE){
        perror("Expected payload is greater than buf size");
      }
      info->state = READ_PAYLOAD; 
      info->payload_bytes_read = 0;
    } else if (info->state == READ_PAYLOAD){
      ssize_t bytes_read = read(info->client_fd,
                                info->payload_buffer + info->payload_bytes_read,
                                info->expected_payload_len - info->payload_bytes_read);
      if(bytes_read == 0){
        // Possibility of payload with 0 bytes
        // So we probably just leave it
      }

      if(bytes_read < 0){
        if(errno == EAGAIN || errno == EWOULDBLOCK){
          // No more data to read
          return 0;
        } else {
          perror("handle_client_read - read");
          return -1;
        }
      }

      info->payload_bytes_read += (size_t) bytes_read;

      if(info->payload_bytes_read < info->expected_payload_len){
        // Exit early because we need more payload bytes
        return 0;
      }

      // If we reach this point, then our payload should be complete
      // Put the stuff into the client's big buffer
      if(append_to_client_buffer(info) == -1){
        perror("append_to_client_buffer");
      }

      // Check if the msg is done
      if(info->msg_done){
        uint8_t *payload_copy = copy_buffer(info->partial_msg, info->partial_len);

        // Process message
        int payload_result = process_payload(state, info, payload_copy);
        free(payload_copy);
        if(payload_result == -1){
          perror("Disconnect");
        }
      }

      // If it's not done, then we just switch back to reading the next header
      // Don't do anything with the payload yet
      info->state = READ_HEADER;
      info->payload_bytes_read = 0;
    } else{
      perror("This should not be possible");
      return -1;
    }
  }
*/
}
