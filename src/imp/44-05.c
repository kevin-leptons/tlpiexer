/*
SYNOPSIS

    Solution for exercise 44-05.

REQUIREMENT

    The server in Listing 44-7 (fifo_seqnum_server.c) performs a second
    O_WRONLY open of the FIFO so that it never sees end-of-file when reading
    from the reading descriptor (serverFd) of the FIFO. Instead of doing this,
    an alternative approach could be tried: whenever the server see end-of-file
    on the reading descriptor, it closes the descriptor, and then once more
    opens the FIFO for reading (This open would block util the next client
    opened the FIFO for writing). What is wrong with this approach ?.

SOLUTION

    In case FIFO is O_NON_BLOCK:

        Close it will lost all data in writing by client and make client 
        receive SIGPIPE or EPIPE

    In all of case, during closing and opening again, client can not open
    write FIFO to server. So server can not serve any clients

AUTHORS

    Kevin Leptons <kevin.leptons@gmail.com>
*/

#include <stdlib.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
    printf("Goto %s to see solution\n", __FILE__);
    return EXIT_SUCCESS;
}
