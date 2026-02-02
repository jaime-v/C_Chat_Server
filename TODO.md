As of Dec 6 2025:

* Plan to do:
    * Create an actually good Github repo for this project
    * Make better error logs and handling (warning, info, error and continue, error and fail, etc.)
    * Properly refactor some headers
    * Create better structure around functions that internally use malloc
    * Add in unique names check
    * Have a timeout so that users that don't send messages for a certain time period are kicked
    * Currently we get an error when fuzzing with 10k clients repeatedly. Heap-buffer-overflow,
        specifically when setting
        ```
        72 - client->msg_queue.tail->next = packet;
        ```
        not sure what the exact issue is though.
    * I think that's it?

As of Feb 1 2026:
    Currently working on two errors
    1. Read of 8193, when copying buffers. Quite the exact number. 
        I think it has to do with my buffer not checking properly for a payload
        that is > BUF_SIZE.
    2. Writes to zero page. I think this is because I somehow have a race condition
        and I am writing to something that is already closed. Occurs when enqueuing
        a message packet to client's message queue

I think solution for problem 1 is to add string safety when allocating 
instead of with the copy. 
Currently, append_to_client_buffer allocates exactly enough memory for the message in
client_info->partial_msg.
Then copy_buffer makes a copy of that partial_msg and adds 1 to its size for the null 
terminator.
So it's possible we try to copy something like 8193 bytes from our src to our dest,
but that can result in error, even though we modify that last byte to be '\0'.
So, solution is to add that string safety when allocating memory in append_to_client_buffer.

