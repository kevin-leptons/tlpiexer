/*
SYNOPSIS

    Solution for exercise 52-01.

REQUIREMENT

    Modify the program in Listing 52-5 (pmsg_receive.c) to accept a timeout
    (a relative number of seconds) on the command line, and use
    mq_timedreceive() instead of mq_receive().

SOLUTION

    Copy program in "tlpi/pmsg/pmsg_receive.c" to 
    "tlpi/pmsg/pmsg_receive_52-1.c"
    then add program into "tlpi/pmsg/Makefile".

    Use getopt() to parse option "-t time" then convert its value to integer.
    Use clock_gettime() to retrieve current time into "struct timespec". 
    Add timeout to timespec.

    Verify options "-n" and "-t time" are not provided together.

    Use mq_timedreceive() instead of mq_receive().

USAGE

        # compile
        $ cd tlpi
        $ make
        $ cd pmsg

        # create and send messages
        $ ./pmsg_create -cx /mq
        $ ./psmg_send /mq "hello kitty" 0
        $ ./psmg_send /mq "hello kitty again" 0
        $ ./psmg_send /mq "hi kitty" 0

        # receive messages
        $ ./psmg_receive_52-1 -n /mq
        $ ./psmg_receive_52-1 -t 3 /mq

        # cause error
        $ ./psmg_receive_52-1 -n -t 3 /mq

AUTHORS

    Kevin Leptons <kevin.leptons@gmail.com>
*/

#include <stdlib.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
    printf("Go to %s to se solution\n", __FILE__);
    return EXIT_SUCCESS;
}
