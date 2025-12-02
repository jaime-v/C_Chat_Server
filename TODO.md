As of Nov 23:

## TODO:
Refactor src/client/client.c
Handle errors properly 
    Main should handle server shutdown
    Worker Functions should do one of two things:
        On nonfatal client error: Clean up client, return NULL; ending the thread.
        On fatal server error: Log and shutdown the server, then return.

* Plan to do:
    * Implement Epoll
    * Create an actually good Github repo for this project
    * Make better error logs and handling (warning, info, error and continue, error and fail, etc.)
    * Properly refactor some headers, functions that internally use malloc

* Currently working on:
    * Epoll
    * Buffers don't reset between messages, so we get John -> JohnHello -> JohnHelloNext message
    * Also, disconnecting a client with ctrl+c will cause infinite loop on server end

## BUGS:
Issue: Clients Joining and Leaving
Description:
    We have 3 Clients A, B, C.
    A and B join the server, they can message each other as normal.
    A leaves. C joins.
    B and C cannot message each other. Address sanitizer says error.
Hints?
    Caused by read access, address points to zero page.
    So we are not cleaning up client list properly?
Solution:
    In remove client from list function in client list file, we were accessing state's
    client count then decrementing. When it should be decrementing the count, then accessing.
    This is because the list is 0-indexed, so the 0'th client info is the first in client count.
    ```
    state->client_count-- 
    // becomes 
    --state->client_count
    ```
