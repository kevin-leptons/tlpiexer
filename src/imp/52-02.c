/*
SYNOPSIS

    Solution for exercise 52-02.

REQUIREMENT

    Recode the sequence-number client-server application of Section 44.8 
    to use POSIX message queues.

SOLUTION

    CLIENT: 52-02-client.c

        Create and open client-queue which use to server write on it.

        Open server-queue and send message contains name of client-queue.

        Wait for server responding, and read message from client-queue. 

        Close both client-queue and server-queue.

        Display sequence number.

    SERVER: 52-02-server.c

        Setup signal handler.
        If process receive SIGINT cause by "Ctrl + C" or SIGTERM
            
            Free server-queue 
            Send its signal again to terminate process

        Create, open server-queue.

        Enter infinite loop for receive request from client-queue. With
        each request call "handl_req()":

            Detect name of client-queue 
            Open client-queue
            Send back response
            Increase "seq_num"
            Close client-queue

USAGE

    $ ./dest/bin/52-02-server &
    [1] 27015

    $ ./dest/bin/52-02-client
    response: 0
    $ ./dest/bin/52-02-client
    response: 1
    $ ./dest/bin/52-02-client
    response: 2

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
