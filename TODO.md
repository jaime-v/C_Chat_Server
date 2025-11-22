As of Nov 14:

## TODO:
Refactor src/client/client.c
Handle errors properly 
    Main should handle server shutdown
    Worker Functions should do one of two things:
        On nonfatal client error: Clean up client, return NULL; ending the thread.
        On fatal server error: Log and shutdown the server, then return.

Workers should not shut down the server unless it's by design like with a shutdown command.



* Plan to do:
    * Implement Client Commands
    * Implement Epoll
    * Create an actually good Github repo for this project
    * Make better error logs and handling (warning, info, error and continue, error and fail, etc.)


Client Commands:
    Send payload to server.
    Before processing payload as a message, see if it is a command.
    Tokenize the command.
    Parse the command.
    Implement command logic.
        /quit should remove client from server state, broadcast announcement, clean up client
        /shutdown should shut down the server, clean up all clients
        /list list out all clients, only the client that used this command should see the list
        /whisper NAME MESSAGE sends MESSAGE to client with name NAME, handle if user not found


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
