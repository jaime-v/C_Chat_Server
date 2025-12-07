#ifndef PROTOCOL_H
#define PROTOCOL_H
#include "io.h"
#include <stdint.h>
#include <stdbool.h>

#define MSG_TYPE_NORMAL 0x00
#define MSG_TYPE_ADMIN  0x01
#define MSG_TYPE_RANDOM 0x02
#define MSG_SIZE_LIMIT 65536

#pragma pack(push, 1)
struct msg_header {
  uint32_t msg_len;
  uint8_t msg_type;
  uint8_t msg_done;
};
#pragma pack(pop)

/**
 * Reads a header from socket fd.
 * Returns number of bytes read, or 0 for reading nothing, or -1 for error.
 *
 * @param fd      File descriptor to read from.
 * @param header  Pointer to struct to store the parsed header.
 *
 * @return        Number of bytes read.
 */
ssize_t read_header(int fd, struct msg_header *header_out);



/**
 * Reads a payload message of size msg_len from socket fd, storing it in payload_out.
 * Returns number of bytes read, or 0 for reading nothing, or -1 for error.
 *
 * @param fd            File descriptor to read from.
 * @param payload_out   Pointer to string to store payload.
 * @param msg_len       Length of payload message.
 *
 * @return              Number of bytes read.
 */
ssize_t read_payload(int fd, char **payload_out, size_t msg_len);



/**
 * Reads an entire packet from socket fd. Stores header in header_out and payload in
 * payload_out.
 * Returns number of bytes read from payload, or 0 for reading nothing, or -1 for error.
 *
 * @param fd            File descriptor to read from.
 * @param header_out    Pointer to struct to store header.
 * @param payload_out   Pointer to string to store payload.
 *
 * @return              Number of bytes read from payload.
 */
ssize_t read_packet(int fd, struct msg_header *header_out, char **payload_out);



/**
 * Writes msg_header header to the socket fd.
 * Returns number of bytes written, or -1 on error.
 *
 * @param fd            File descriptor to write to.
 * @param header        Pointer to header to write.
 *
 * @return              Number of bytes written to fd.
 */
ssize_t write_header(int fd, const struct msg_header *header);



/**
 * Writes a string payload, of length msg_len, to the socket fd.
 * Returns number of bytes written, or -1 for error.
 *
 * @param fd            File descriptor to read from.
 * @param payload       Pointer to string to write.
 * @param msg_len       Length of payload message.
 *
 * @return              Number of bytes written to fd.
 */
ssize_t write_payload(int fd, const char *payload, size_t msg_len);



/**
 * Writes an entire packet to socket fd. 
 * Returns number of bytes written from payload, or -1 on error.
 *
 * @param fd            File descriptor to read from.
 * @param header        Pointer to struct header to write.
 * @param payload       Pointer to string to write.
 * @param msg_len       Length of payload message.
 *
 * @return              Number of bytes written to fd from payload.
 */
ssize_t write_packet(
    int fd, 
    const struct msg_header *header, 
    const char *payload, 
    size_t msg_len
);


/**
 *  Initializes header_out with msg_len, msg_type, and msg_done.
 *  Modifies header_out internally.
 *
 *  @param header_out   Pointer to msg_header.
 *  @param msg_len      Length of message.
 *  @param msg_type     Type of message.
 *  @param msg_done     Bool for if message continues.
 *
 *  @return             0 on success, -1 on failure
 */
int create_header(
    struct msg_header *header_out,
    size_t msg_len,
    uint8_t msg_type,
    bool msg_done
);


#endif
