/*
SYNOPSIS
    
    Solution for exercise 44-04.

REQUIREMENT

    Add code to the server in Listing 44-7 (fifo_seqnum_server.c) so that if
    the program receives the SIGINT or SIGTERM signals, it removes the server
    FIFO and terminates.

SOLUTION

    - Copy tlpi/pipes/fifo_seqnum_server.c to 
      tlpi/pipes/fifo_seqnum_server_44-4.c
    - Modify program as requirements

USAGE

    $ ./dest/bin/44-04
    Goto ... to see solution

AUTHOR

    Kevin Leptons <kevin.leptons@gmail.com>
*/

#include <stdlib.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
    printf("Goto %s to see solution\n", __FILE__);
    return EXIT_SUCCESS;
}
