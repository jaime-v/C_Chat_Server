#ifndef PROCESS_PAYLOAD_H
#define PROCESS_PAYLOAD_H

#include "server_state.h"
#include <stdint.h>

int process_payload(struct server_state *state, struct client_info *info, uint8_t *payload_copy);

#endif
