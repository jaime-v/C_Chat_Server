#ifndef COMMAND_H
#define COMMAND_H

#include <stddef.h>

enum CMD {
  CMD_QUIT,
  CMD_LIST,
  CMD_WHISPER,
  CMD_SHUTDOWN,
  CMD_UNKNOWN
};

enum CMD_RES {
  CMD_OK,
  CMD_DISCONNECT,
  CMD_ERROR,
  CMD_WARNING
};

struct command_entry{
  const char *name;
  enum CMD cmd;
};

enum CMD find_command(const char *cmd);

#endif
