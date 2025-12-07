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

