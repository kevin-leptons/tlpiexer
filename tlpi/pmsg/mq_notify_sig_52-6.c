/*************************************************************************\
*                  Copyright (C) Michael Kerrisk, 2016.                   *
*                                                                         *
* This program is free software. You may use, modify, and redistribute it *
* under the terms of the GNU General Public License as published by the   *
* Free Software Foundation, either version 3 or (at your option) any      *
* later version. This program is distributed without any warranty.  See   *
* the file COPYING.gpl-v3 for details.                                    *
\*************************************************************************/

/* Listing 52-6 */

/* mq_notify_sig.c

   Usage: mq_notify_sig mq-name

   Demonstrate message notification via signals (catching the signals with
   a signal handler) on a POSIX message queue.
*/
#include <signal.h>
#include <mqueue.h>
#include <fcntl.h>              /* For definition of O_NONBLOCK */
#include "tlpi_hdr.h"

#define NOTIFY_SIG SIGUSR1

void handle_signal(int sig)
{
    // do nothing
}

/* This program does not handle the case where a message already exists on
   the queue by the time the first attempt is made to register for message
   notification. In that case, the program would never receive a notification.
   See mq_notify_via_signal.c for an example of how to deal with that case. */

int
main(int argc, char *argv[])
{
    struct sigevent sev;
    mqd_t mqd;
    struct mq_attr attr;
    void *buffer;
    ssize_t numRead;
    sigset_t wait_mask;
    siginfo_t wait_info;

    if (argc != 2 || strcmp(argv[1], "--help") == 0)
        usageErr("%s mq-name\n", argv[0]);

    mqd = mq_open(argv[1], O_RDONLY | O_NONBLOCK);
    if (mqd == (mqd_t) -1)
        errExit("mq_open");

    /* Determine mq_msgsize for message queue, and allocate an input buffer
       of that size */

    if (mq_getattr(mqd, &attr) == -1)
        errExit("mq_getattr");

    buffer = malloc(attr.mq_msgsize);
    if (buffer == NULL)
        errExit("malloc");

    /* Register for message notification via a signal */
    sev.sigev_notify = SIGEV_SIGNAL;
    sev.sigev_signo = NOTIFY_SIG;
    sev.sigev_value.sival_ptr = &mqd;

    sigemptyset(&wait_mask);
    sigaddset(&wait_mask, NOTIFY_SIG);
    if (signal(NOTIFY_SIG, handle_signal) == SIG_ERR)
        errExit("signal");

    for (;;) {
        /* Reregister for message notification */
        if (mq_notify(mqd, &sev) == -1)
            errExit("mq_notify");

        while ((numRead = mq_receive(mqd, buffer, attr.mq_msgsize, NULL)) >= 0)
            printf("Read %ld bytes\n", (long) numRead);

        // wait for NOTIFY_SIG
        if (sigwaitinfo(&wait_mask, &wait_info) == -1)
            errExit("sigwaitinfo");

        // NOTIFY_SIG has been received
        // display information
        printf("Receive signal=%s, mqd=%i\n", 
                strsignal(wait_info.si_signo), 
                *(mqd_t*)wait_info.si_value.sival_ptr);

        if (errno != EAGAIN)            /* Unexpected error */
            errExit("mq_receive");
    }
}
