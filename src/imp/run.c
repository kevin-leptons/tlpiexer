#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <inttypes.h>
#include <unistd.h>
#include <limits.h>

#include <tlpiexer/error.h>

int main(void)
{
    printf("PIPE_BUF=%i\n", PIPE_BUF);

    return EXIT_SUCCESS;
}
