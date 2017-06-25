/*
SYNOPSIS

    Solution for exercise 52-03.

REQUIREMENT

    Rewrite file-server application of Section 46.8 to use POSIX message queues
    instead of System V message queues

SOLUTION

    SERVER

    Setup handler for:

        SIGINT cause by "Ctrl + C"
        SIGTERM occur when error isn't handle

    Handler perform free message queues of server:

        Unlink server-queue
        Reset signal handler to default
        Send signal again to terminate program

    Open server-queue. Enter infinite loop to receive message from client.
    With each request, call "handle_req()":

        Detect file which requested
        Open file
        Open client-queue and check for client message size
        Read file by block, then send to client-queue until end of file
        Close file
        Close client-queue

    CLIENT

    Setup signal handler for:

        SIGINT is cause by "Ctrl +V"
        SIGTERM is cause by error without handler

    Signal handler do:

        Close client-queue
        Unlink client-queue
        Reset signal handler to default
        Send signal again to terminate process

    Open client-queue to server write message on it.

    Open server-queue and check for its message size.

    Create message with file name and client-queue name then send to
    server-queue.

    Enter loop to read messages from client-queue and print it to stdout
    until received end-file message.

    Free resource

        Close server-queue
        Close client-queue
        Remove client-queue

USAGE

    $ ./dest/bin/52-03-server &
    [1] 13841

    $ echo "hello there" > tmp/data.txt
    $ ./dest/bin/52-03-client tmp/data.txt
    hello there

    $ kill %1

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
