#include "command.h"
#include "command_table.h"
#include <string.h>

enum CMD find_command(const char *cmd){
  for (int i = 0; command_table[i].name != NULL; i++){
    if(strcmp(command_table[i].name, cmd) == 0){
      return command_table[i].cmd;
    }
  }
  return CMD_UNKNOWN;
}

