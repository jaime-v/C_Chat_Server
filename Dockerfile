FROM ubuntu:22.04

RUN apt-get update && apt-get install -y \
    build-essential \
    make

WORKDIR /app

COPY . .

RUN make

CMD ["./build/server/server"]

