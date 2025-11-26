#ifndef SERVER_HANDLE_COMMAND_H
#define SERVER_HANDLE_COMMAND_H

#include "command.h"
#include "server_state.h"
#include "client_info.h"

enum CMD_RES server_handle_command(
    struct server_state *state, 
    struct client_info *user, 
    char *cmd, 
    char *saveptr
);
enum CMD_RES handle_list(
    struct server_state *state, 
    struct client_info *user
);
enum CMD_RES handle_whisper(
    struct server_state *state, 
    struct client_info *user, 
    char *saveptr
);
enum CMD_RES handle_shutdown(
    struct server_state *state
);
enum CMD_RES server_handle_unknown();

#endif
