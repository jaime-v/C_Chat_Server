#ifndef CLIENT_HANDLE_COMMAND_H
#define CLIENT_HANDLE_COMMAND_H

#include "command.h"
#include "listen_info.h"

enum CMD_RES client_handle_command(int sfd /*, struct listen_info *info */, char *cmd);
enum CMD_RES handle_quit(int sfd /* , struct listen_info *info */);
enum CMD_RES client_handle_unknown();

#endif
