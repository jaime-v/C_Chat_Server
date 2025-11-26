#include "client_setup.h"
#include "logging.h"
#include "io.h"
#include "protocol.h"
#include "common_header.h"
#include "common_thread.h"
#include "client_listen.h"
#include "command.h"
#include "client_handle_command.h"
#include "utils.h"
#include <string.h>
#include <pthread.h>

int main(void){
  int sfd;
  struct sockaddr_in addr;
  ssize_t bytes_read;
  char buf[BUF_SIZE];
  bool last_chunk_filled = false;

  // Init client socket
  if(init_client(&sfd, &addr) == -1){
    handle_error("init_client");
  }


  // Init listen_info struct (not sure if we need this tbh, we can just pass sfd *)
  struct listen_info *info = (struct listen_info *)malloc(sizeof(struct listen_info));
  info->sfd = sfd;

  /* NOTE - Bad to do this because listen can't close on its own
  // Create and detach thread for listening
  if(create_and_detach_thread(client_listen, (void *)info) == -1){
    // Handle Error
  }
  */ 

  pthread_t thread;
  if(pthread_create(&thread, NULL, client_listen, (void *)info) != 0){
    handle_error("pthread_create");
  }

  // Prompt
  printf("Enter your username as your first message: \n");

  while((bytes_read = read(STDIN_FILENO, buf, BUF_SIZE)) > 0){
    // Set up a header
    struct msg_header header;
    header.msg_len = htonl((uint32_t)bytes_read);
    header.msg_type = MSG_TYPE_NORMAL;
    header.msg_done = false;

    // Check if msg is command
    if(buf[0] == '/'){
      // Create a copy and null terminate it -- malloc'd internally, need to free
      char *buf_copy = (char *)malloc((size_t)bytes_read);
      memcpy(buf_copy, buf, (size_t)bytes_read);
      buf_copy[bytes_read - 1] = '\0';

      // Tokenize the command
      char *saveptr;
      char *cmd = strtok_r(buf_copy, " ", &saveptr);
      char *rest_of_message = saveptr;
      if(rest_of_message != NULL){
        printf("theres args in the cmd\n");
      }

      // Make command lowercase
      size_t cmd_len = strlen(cmd);
      make_lowercase(cmd, cmd_len);

      enum CMD_RES command_result = client_handle_command(sfd, /*info,*/ cmd);

      free(buf_copy);
      if(command_result == CMD_ERROR || command_result == CMD_DISCONNECT){
        printf("Disconnect or error\n");
        break;
      }
    }

    // If we only read 1 byte, it is a \n, so don't send anything
    if(bytes_read == 1 && last_chunk_filled == false){
      printf("Only have newline\n");
      continue;
    }

    // Scan the buffer, if there is a \n, then the message is done
    if(memchr(buf, '\n', BUF_SIZE) != NULL){
      // Set header flag to be done, last chunk filled to be false
      header.msg_done = true;
      last_chunk_filled = false;
      printf("Found end of message\n");

      // Sanitize input
      // We read in the message + the \n character, but we only want to send message.
      // We change the \n to a \0, but we don't send the \0
      // Hello\n (6 bytes) 0,1,2,3,4,5 -- bytes_read = 6
      // Change 5 (\n) into (\0)
      // bytes_read -= 1 ->  -- bytes_read = 5
      // buf[bytes_read] = \0
      // or, just buf[--bytes_read] = '\0';
      // and make sure to adjust the header again
      buf[--bytes_read] = '\0';
      header.msg_len = htonl((uint32_t)bytes_read);

    } else {
      // Otherwise, message is not done, and it expands past BUF_SIZE, so we set a flag that we are 
      // continuing the message - in case it is exactly BUF_SIZE bytes, and the newline character 
      // will be in the next buffer
      printf("Message continues\n");
      last_chunk_filled = true;
    }
 
    // Send header
    if(write_header(sfd, (const struct msg_header *)&header) == -1){
      handle_error("write - header");
    }
    
    // Then we send payload
    // bytes_read does not include the \0
    // e.g. Hello\0 - bytes_read = 5, just for Hello
    if(write_payload(sfd, (const char *)buf, (size_t)bytes_read) == -1){
      handle_error("write - payload");
    }
  }

  if(bytes_read == 0){
    printf("read in EOF\n");
  }

  if(bytes_read == -1){
    printf("[client] read error\n");
  }

  /*
  if(close(sfd) == -1){
    handle_error("close");
  }
  */

  free(info);
  printf("\n\n[client] Joining with listen thread\n\n");
  pthread_join(thread, NULL);
  printf("\n\n[client] Joined with listen thread, returning now\n\n");

  return 0;
}
