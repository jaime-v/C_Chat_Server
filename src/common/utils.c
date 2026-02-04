#include "utils.h"
#include <stdlib.h>
#include <string.h>
#include <time.h>

int get_timestamp(char *timestamp_out, size_t size) {
  time_t now = time(NULL);
  struct tm *tm_info = localtime(&now);
  if (tm_info == NULL) {
    return -1;
  }
  strftime(timestamp_out, size, "[%H:%M:%S]", tm_info);
  return 0;
}

uint8_t *copy_buffer(uint8_t *buf_src, size_t size) {
  uint8_t *buf_copy_out = (uint8_t *)malloc(size + 1);
  if (buf_copy_out == NULL) {
    return NULL;
  }
  memcpy(buf_copy_out, buf_src, size);
  buf_copy_out[size] = '\0';
  return buf_copy_out;
}

void make_lowercase(char *src, size_t size) {
  for (size_t i = 0; i < size; i++) {
    if (src[i] >= 'A' && src[i] <= 'Z') {
      src[i] += ('a' - 'A');
    }
  }
}
