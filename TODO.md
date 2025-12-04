As of Dec 2 2025:

Ima be so real, i never use this.
but we need to work on partial writes i think.
just like how we adjusted the partial reads, we need to do the same for writes.
that might be why our fuzzers are failing sometimes. if we have a partial write it cant
read the header or the payload. and everyone instantly fails and gets marked.

* Plan to do:
    * Create an actually good Github repo for this project
    * Make better error logs and handling (warning, info, error and continue, error and fail, etc.)
    * Properly refactor some headers
    * Create better structure around functions that internally use malloc
    * Add in unique names check
    * Have a timeout so that users that don't send messages for a certain time period are kicked
    * Remove size\_t and replace with uint\_* fixed with integers
    * Possibly pack header bytes?

* High Priority:
    * Robust header parsing
        - Reject invalid msg\_len, msg\_type, msg\_done, misaligned short reads
        - Handle partial headers (should be fine with epoll)
        - Disconnect instantly on bad header
    * Better error logging
        - Info
        - Warn
        - Error
        - Debug
        - Maybe something else? i think that covers it
    * Unique Usernames
    * Usage of fixed-width integers

* Medium Priority:
    * Timeout system
    * Message header packing 

* Low Priority:
    * Proper memory pooling with custom allocator?
    * Rate limiting
