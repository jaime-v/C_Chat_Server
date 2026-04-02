#include <arpa/inet.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define MAX_PAYLOAD 4096

typedef struct {
  char *ip;
  int port;
  int messages;
  int delay_us;
} fuzz_args;

void *fuzz_client(void *arg) {
  fuzz_args *fa = (fuzz_args *)arg;

  int fd = socket(AF_INET, SOCK_STREAM, 0);
  if (fd < 0)
    return NULL;

  struct sockaddr_in addr = {0};
  addr.sin_family = AF_INET;
  addr.sin_port = htons(fa->port);
  inet_pton(AF_INET, fa->ip, &addr.sin_addr);

  if (connect(fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
    close(fd);
    return NULL;
  }

  for (int i = 0; i < fa->messages; i++) {
    int len = rand() % MAX_PAYLOAD + 1;

    uint8_t buf[MAX_PAYLOAD];
    for (int j = 0; j < len; j++)
      buf[j] = rand() % 256;

    send(fd, buf, len, 0);

    if (fa->delay_us > 0)
      usleep(fa->delay_us);
  }

  close(fd);
  return NULL;
}

int main(int argc, char **argv) {
  srand(time(NULL));

  char *ip = "127.0.0.1";
  int port = 8000;
  int clients = 10000;
  int messages = 50;
  int delay = 1;

  pthread_t threads[clients];

  fuzz_args fa = {
      .ip = ip, .port = port, .messages = messages, .delay_us = delay};

  for (int i = 0; i < clients; i++)
    pthread_create(&threads[i], NULL, fuzz_client, &fa);

  for (int i = 0; i < clients; i++)
    pthread_join(threads[i], NULL);

  return 0;
}
