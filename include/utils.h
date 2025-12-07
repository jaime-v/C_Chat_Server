#ifndef UTILS_H
#define UTILS_H

#include <sys/types.h>
#include <stdint.h>

/**
 * Gets timestamp and stores it in timestamp_out.
 *
 * @param timestamp_out   Buffer to store timestamp. Expected to be modified in function.
 * @param len             Length of buffer.
 *
 * @return                0 on success, -1 on failure.
 */
int get_timestamp(char *timestamp_out, size_t len);

/**
 * Creates and returns a new buffer with size bytes and buf_src copied into it.
 * Locally null terminates.
 *
 * @param buf_src       Buffer to copy from.
 * @param size          Size of buffer to allocate.
 *
 * @return              The newly allocated buffer on success, NULL on failure.
 */ 
uint8_t *copy_buffer(uint8_t *buf_src, size_t size);


/**
 *  Takes a string src and makes all characters lowercase.
 *
 *  @param src        String to make lowercase.
 *  @param size       Size of string.
 *
 *  @return           Void function returns nothing.
 */
void make_lowercase(char *src, size_t size);

#endif
