#include "io.h"
#include <errno.h>

ssize_t read_all(int fd, void *buffer, size_t count){
  size_t total_read = 0;
  char *ptr = buffer;
  while(total_read < count){
    ssize_t r = read(fd, ptr + total_read, count - total_read);
    if (r < 0){
      if(errno == EINTR) { continue; }
      return -1;
    }
    if (r == 0) { break; }
    total_read += (size_t)r;
  }
  return (ssize_t)total_read;
}

ssize_t write_all(int fd, const void *buffer, size_t count){
  size_t total_written = 0;
  const char *ptr = buffer;
  while(total_written < count){
    ssize_t w = write(fd, ptr + total_written, count - total_written);
    if (w < 0){
      if(errno == EINTR) { continue; }
      return -1;
    }
    if (w == 0) { break; }
    total_written += (size_t)w;
  }
  return (ssize_t)total_written;
}

