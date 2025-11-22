#ifndef CLIENT_THREAD_ARGS_H
#define CLIENT_THREAD_ARGS_H

#include "server_state.h"
#include "client_info.h"

struct client_thread_args {
  struct server_state *state;
  struct client_info *info;
};

#endif
