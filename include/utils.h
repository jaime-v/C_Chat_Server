#ifndef UTILS_H
#define UTILS_H

#include <sys/types.h>

/**
 * Gets timestamp and stores it in timestamp_out.
 *
 * @param timestamp_out   Buffer to store timestamp. Expected to be modified in function.
 * @param len             Length of buffer.
 *
 * @return                0 on success, -1 on failure.
 */
int get_timestamp(char *timestamp_out, size_t len);

#endif
