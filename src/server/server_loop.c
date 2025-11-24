#include "client_info.h"
#include "common_thread.h"
#include "handle_client.h"
#include "client_thread_args.h"
#include "client_info_init.h"
#include "client_list.h"
#include "server_control.h"
#include "broadcast.h"
#include <sys/socket.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <netinet/in.h>
#include <string.h>

// Debug
#include <errno.h>

int server_loop(struct server_state *state){
  int cfd;
  struct sockaddr_in client_addr;
  socklen_t client_addr_size;

  for(;;){
    // Accept incoming client
    cfd = accept(state->server_fd, (struct sockaddr *)&client_addr, &client_addr_size);
    if(cfd == -1){
      printf("[server_loop] Accept failed errno: %d\n", errno);
      if(server_shutdown == 1){
        printf("[server_loop] FOUND SHUTDOWN FLAG %p = %d\n", (void *)&server_shutdown, 
            server_shutdown);
        break;
      } else {
        printf("[server_loop] Continuing then\n");
        continue;
      }
      // return -1;
      // We don't really want to return -1 on a failure, we want to continue trying to accept
      // This error is not worthy of a shutdown
    }

    // Create client_info struct
    struct client_info *info = (struct client_info *)malloc(sizeof(struct client_info));
    if(client_info_init(info, cfd) == -1){
      return -1;
    }

    // Create client's thread
    struct client_thread_args *args = malloc(sizeof(struct client_thread_args));
    args->state = state;
    args->info = info;
    if(pthread_create(&info->thread, NULL, handle_client, (void *)args) != 0){
      return -1;
    }
    /*
    if(create_and_detach_thread(handle_client, (void *)args) == -1){
      return -1;
    }
    */

    if(add_client_to_list(state, info) == -1){
      printf("Couldn't add client\n");
    }
  }

  char *shutdown_message = "SERVER SHUTDOWN";
  size_t shutdown_len = strlen(shutdown_message);
  broadcast(state, NULL, shutdown_message, shutdown_len + 1);

  for(size_t i = 0; i < state->client_count; i++){
    shutdown(state->client_list[i]->cfd, SHUT_RDWR);
    pthread_join(state->client_list[i]->thread, NULL);
  }

  if(remove_all_clients(state) == -1){
    // Which we might not want? we want to retry this
    return -1;
  }

  printf("[server_loop] holy shit we reached the end\n");
  return 0;
}
