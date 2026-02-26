# High-Performance TCP Chat Server (C, epoll)

This project is a multi-client chat server implemented in C using Linux epoll for scalable event-driven I/O.  
It supports a custom binary messaging protocol, partial read/write handling, and stress testing with fuzzing clients.

---

## Features

- Event-driven TCP server using epoll
- Custom binary protocol (fixed-size header + payload)
- Partial read/write handling with per-client state
- Message broadcasting and command handling
- Stress tested with custom fuzzers (1000+ clients)
- Dockerized Linux environment for portability

---

## Requirements

- Docker

---

## Build

```
docker build -t chat-server .
```

## Run Server
```
docker run -p 8000:8000 chat-server
```

## Run Client
```
docker ps
docker exec -it <container_id> /bin/bash
./build/client/client
```

## Run Fuzzer
```
docker ps
docker exec -it <container_id> /bin/bash
cd tests
gcc <fuzzer_file.c>
./a.out
```

## Cleanup
```
docker container prune
docker image prune
```
