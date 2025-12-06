#ifndef MSG_QUEUE_H 
#define MSG_QUEUE_H

#include "msg_packet.h"

struct msg_queue{
  struct msg_packet *head;
  struct msg_packet *tail;
  size_t queued_bytes;
  size_t queued_count;
};

#endif
