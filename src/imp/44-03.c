/*
SYNOPSIS

    Solution for exercise 44-03.

REQUIRE

    The server in Listing 44-7 (fifo_seqnum_server.c) always start assigning
    sequence numbers from 0 each time it started. Modify the program to use
    a backup file that is updated each time a sequence number is assigned
    (The open() O_SYNC flag, described in Section 4.3.1, may be useful).
    At startup, the program should check for the existence of this file, and
    if it is present, use the value it contains to initialize the sequence
    number. If the backup file can't be found on startup, the program should
    create a new file and start assigning sequence numbers beginning at 0
    (An alternative to this technique would be to use memory-mapped files,
    described in Chapter 49).

SOLUTION

    - Copy tlpi/pipes/fifo_seqnum_server.c to 
      tlpi/pipes/fifo_seqnum_server_44-3.c
    - Modify program as requirements

USAGE

    $ ./dest/bin/44-03
    Goto ... to see solution

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
