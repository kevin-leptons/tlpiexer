/*
SYNOPSIS

    Solution for exercise 52-06.

REQUIREMENT

    Replace the use of a signal handler in Listing 52-6 (mq_notify_sig.c) with
    the use of sigwaitinfo(). Upon return from sigwaitinfo(), display the
    values in the returned siginfo_t structure. How could the program obtain
    the message queue descriptor in the siginfo_t structure returned by
    sigwaitinfo()?.

SOLUTION

    Copy source in "tlpi/pmsg/mq_notify_sig.c" to 
    "tlpi/pmsg/mq_notify_sig_52-6.c". Then add program to
    "tlpi/pmsg/Makefile".

    Remove usage of "sigaction()" by "sigwaitinfo()" below "mq_notify()".

    How to the program obtain message queue descriptor ?. 
    Assign struct sigevent_t.sigev_value to message queue descriptor and get
    its value from siginfo_t.si_value from sigwaitinfo().

USAGE       

    # compile
    $ cd tlpi/pmsg
    $ make

    # create queue
    $ ./pmsg_create -cx /mq-notify

    # run modified program
    $ ./mq_notify_sig_52-6 /mq-notify

    # send first message to queue
    $ ./pmsg_send /mq-notify "hello kitty"
    Receive signal=User defined signal 1, mqd=123
    hello kity

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
    printf("Go to %s to see solution\n", __FILE__);
    return EXIT_SUCCESS;
}
