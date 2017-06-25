/*
SYNOPSIS

    Solution for exercise 63-02.

REQUIREMENT

    Write an echo server (see Section 60.2 and 60.3) that handles both
    TCP and UDP clients. To do this, the server must create both a listening
    TCP socket and a UDP socket, and then monitor both sockets using one
    of techniques described in this chapter.

SOLUTION

    Parse command line to get port number which server listen on.

    Create TCP, UDP socket by socket().

    Bind two sockets above to specific port by bind().

    Mark two sockets are listen by listen().

    Use epoll() to monitor two socket.

    With each connections comming, accept it, create new process to handle
    it.

USAGE

    # start server in background
    $ ./dest/bin/63-01-server 8080 &> tmp/server.log &
    [1] 3245

    # try udp message
    $ ./dest/bin/63-01-client localhost 8080 udp "hello"
    hello you

    # try tcp mesage
    $ ./dest/bin/63-01-client localhost 8080 tcp "nice to meet you"
    nice to meet you

    # stop server
    kill %1

    # see server log
    cat tmp/server.log
    Listen on port 8080 for UDP, TCP
    ... => UDP: hello
    ... => TCP: nice to meet you

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
