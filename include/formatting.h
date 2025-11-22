#ifndef FORMATTING_H
#define FORMATTING_H

#include "client_info.h"

/**
 *  Copies name (stored in client's buffer) into client's name field.
 *  If length of name > NAME_MAX, name is truncated to the first NAME_MAX bytes.
 *
 *  @param client   Pointer to client's info struct.
 *
 *  @return         0 on success -- might be void able
 */
int store_client_name(struct client_info *client);

/**
 *  Formats client's buffer into a chat message.
 *
 *  @param client   Pointer to client's info struct.
 *
 *  @return         Formatted message on success, NULL on failure.
 */
char *format_chat_message(const struct client_info *client);

#endif
