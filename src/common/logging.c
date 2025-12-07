#include "logging.h"

// This definitely needs to be better...

void handle_error(char *msg){
  perror(msg);
  exit(EXIT_FAILURE);
}
