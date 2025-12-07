#ifndef FORMATTING_H
#define FORMATTING_H

#include "client_info.h"

/**
 *  Formats client's buffer into a chat message.
 *
 *  @param client   Pointer to client's info struct. Should have everything we need.
 *
 *  @return         Formatted message on success, NULL on failure.
 */
uint8_t *format_chat_message(const struct client_info *client);

/**
 *  Formats incoming message into a whisper message.
 *
 *  @param sender   Pointer to sender's client_info struct.
 *  @param msg      Message to format.
 *
 *  @return         Formatted message on success, NULL on failure.
 */
uint8_t *format_whisper_message(const struct client_info *sender, const uint8_t *msg, size_t msg_len);

#endif
