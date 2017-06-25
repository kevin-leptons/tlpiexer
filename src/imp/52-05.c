/*
SYNOPSIS

    Solution for exercise 52-05.

REQUIREMENT

    Modify the program in Listing 52-6 (mq_notify_sig.c) to demonstrate that
    message notification established by mq_notify() occurs just once. This can
    be done by removing the mq_notify() call inside the for loop.

SOLUTION

    Copy source in "tlpi/pmsg/mq_notify_sig.c" to 
    "tlpi/pmsg/mq_notify_sig_52-5.c". Then add program to
    "tlpi/pmsg/Makefile"

USAGE

    # compile
    $ cd tlpi/pmsg
    $ make

    # create queue
    $ ./pmsg_create -cx /mq-notify

    # run modified program
    $ ./mq_notify_sig_52-5 /mq-notify

    # send first message to queue
    $ ./pmsg_send /mq-notify "hello kitty"
    Receive signal SIGUSR1
    Read 11 bytes

    # send second message to queue, now program was exited
    # so program won't print anything
    $ ./pmsg_send /mq-notify "bye kitty"

    # unlink queue
    ./pmsg_unlink /mq-notify

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
