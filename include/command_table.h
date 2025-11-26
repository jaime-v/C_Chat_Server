#ifndef COMMAND_TABLE_H
#define COMMAND_TABLE_H

static struct command_entry command_table[] = {
  { "/quit",      CMD_QUIT },
  { "/list",      CMD_LIST },
  { "/whisper",   CMD_WHISPER },
  { "/shutdown",  CMD_SHUTDOWN },
  { NULL,         CMD_UNKNOWN },
};

#endif
