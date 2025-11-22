#ifndef IO_H
#define IO_H

#include <unistd.h>

/**
 *  Read wrapper. Ensures we read count bytes from fd into buffer.
 *
 *  @param fd       File descriptor to be read from.
 *  @param buffer   Buffer to read in and store values.
 *  @param count    Number of bytes to read.
 *
 *  @return         Number of bytes actually read.
 */
ssize_t read_all(int fd, void *buffer, size_t count);


/**
 *  Write wrapper. Ensures we write count bytes from buffer into fd.
 *
 *  @param fd       File descriptor to be write.
 *  @param buffer   Buffer to write from.
 *  @param count    Number of bytes to write.
 *
 *  @return         Number of bytes actually written.
 */
ssize_t write_all(int fd, const void *buffer, size_t count);

#endif
