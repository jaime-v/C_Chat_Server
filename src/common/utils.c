#include "utils.h"
#include <time.h>

int get_timestamp(char *timestamp_out, size_t size){
  time_t now = time(NULL);
  struct tm *tm_info = localtime(&now);
  if(tm_info == NULL){
    return -1;
  }
  strftime(timestamp_out, size, "[%H:%M:%S]", tm_info);
  return 0;
}
