/*
SYNOPSIS

    Solution for exercise 44-06.

REQUIREMENT

    The server in Listing 44-7 (fifo_seqnum_server.c) assumes that the client
    process is well behaved. If a misbehaving client created a client FIFO
    and sent a request to the server, but didi not open its FIFO, then the
    server's attempt to open the client FIFO would block, and other client's
    request would be indefinitely delayed (If done maliciously, this would
    constitute ad denial-of-service attack). Devise a scheme to deal with this
    proplem. Extend the server (and possibily the client in Listing 44-8)
    acoordingly

SOLUTION

    Copy tlpi/pipes/fifo_seqnum_server.c to
    tlpi/pipes/fifo_seqnum_server_44-6.c.
    Try open client FIFO by O_NONBLOCK flag, if client FIFO is not exited,
    it will return ENXIO. Hand for this error.

    Copy tlpi/pipes/fifo_seqnum_clien.c to
    tlpi/pipes/fifo_seqnum_client_44-6.c.
    Remove all of codes which open client FIFO

USAGE

    $ cd tlpi
    $ make
    $ cd pipes
    $ fifo_seqnum_serer_44-6 &

    # try to run normal client in few times,
    # it will responses sequence numbers
    $ fifo_seqnum_client
    0
    $ fifo_seqnum_client
    1
    $ fifo_seqnum_client
    2

    # try to run bad client, server will give error message
    # but it still continue running and accept other clients
    $ fifo_seqnum_client_44-6

    # try to run normal client one times again
    $ fifo_seqnum_client
    3

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
