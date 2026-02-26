* Plan to do:
    * Profiling with perf
    * Debugging in container with gdb
    * Create a good Github repo for this project
    * Make better error logs and handling (warning, info, error and continue, error and fail, etc.)
    * Properly refactor some headers
    * Create better structure around functions that internally use malloc
    * Add in unique names check
    * Have a timeout so that users that don't send messages for a certain time period are kicked

* Current issues:
    * Currently we get an error when fuzzing with 10k clients repeatedly. Heap-buffer-overflow,
        specifically when setting
        ```
        72 - client->msg_queue.tail->next = packet;
        ```
        not sure what the exact issue is though.
    * Sometimes memory leaks when fuzzing many clients, not consistently reproducable yet
