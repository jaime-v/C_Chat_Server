#ifndef LOGGING_H
#define LOGGING_H
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

/**
 *  Error handling function if you couldn't guess.
 *  Exits program.
 *
 *  @param msg    Message to print out as error.
 */
void handle_error(char *msg);

#endif
