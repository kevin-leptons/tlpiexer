#include <tlpiexer/error.h>

#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void exit_err(const char *caller)
{
    fprintf(stderr, "Error: %s:%s\n", caller, strerror(errno));
    exit(EXIT_FAILURE);
}

void exit_usage(const char *msg)
{
    fprintf(stderr, "Error: %s\n", msg);
    exit(EXIT_FAILURE);
}

void msg_err(const char *caller)
{
    fprintf(stderr, "Error: %s:%s\n", caller, strerror(errno));
}
