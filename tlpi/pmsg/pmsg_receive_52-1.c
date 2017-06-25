/*************************************************************************\
*                  Copyright (C) Michael Kerrisk, 2016.                   *
*                                                                         *
* This program is free software. You may use, modify, and redistribute it *
* under the terms of the GNU General Public License as published by the   *
* Free Software Foundation, either version 3 or (at your option) any      *
* later version. This program is distributed without any warranty.  See   *
* the file COPYING.gpl-v3 for details.                                    *
\*************************************************************************/

/* Listing 52-5 */

/* pmsg_receive.c

   Usage as shown in usageError().

   Receive a message from a POSIX message queue, and write it on
   standard output.

   See also pmsg_send.c.

   Linux supports POSIX message queues since kernel 2.6.6.
*/
#include <mqueue.h>
#include <fcntl.h>              /* For definition of O_NONBLOCK */
#include <time.h>
#include <stdbool.h>
#include "tlpi_hdr.h"

static void
usageError(const char *progName)
{
    fprintf(stderr, "Usage: %s [-n] mq-name\n", progName);
    fprintf(stderr, "    -n           Use O_NONBLOCK flag\n");
    fprintf(stderr, "    -t time      Use timeout for each receive call\n");
    exit(EXIT_FAILURE);
}

int
main(int argc, char *argv[])
{
    int flags, opt;
    mqd_t mqd;
    unsigned int prio;
    void *buffer;
    struct mq_attr attr;
    ssize_t numRead;
    struct timespec abs_timeout;
    struct timespec *abs_timeout_p = NULL;
    bool is_nonblock = false;

    flags = O_RDONLY;
    while ((opt = getopt(argc, argv, "nt:")) != -1) {
        switch (opt) {
            case 'n':   
                flags |= O_NONBLOCK;
                is_nonblock = true;
                break;
            case 't': 
                if (clock_gettime(CLOCK_REALTIME, &abs_timeout) != 0)
                    errExit("clock_gettime");
                abs_timeout.tv_sec += atoi(optarg);
                abs_timeout_p = &abs_timeout;
                break;
            default:    
                usageError(argv[0]);
        }
    }

    if (optind >= argc)
        usageError(argv[0]);

    // don't accept both O_NONBLOCK and timeout
    if (is_nonblock && abs_timeout_p != NULL)
        errExit("Do not use both O_NONBLOCK and timeout");

    mqd = mq_open(argv[optind], flags);
    if (mqd == (mqd_t) -1)
        errExit("mq_open");

    /* We need to know the 'mq_msgsize' attribute of the queue in
       order to determine the size of the buffer for mq_receive() */

    if (mq_getattr(mqd, &attr) == -1)
        errExit("mq_getattr");

    buffer = malloc(attr.mq_msgsize);
    if (buffer == NULL)
        errExit("malloc");

    numRead = mq_timedreceive(mqd, buffer, attr.mq_msgsize, &prio, 
            abs_timeout_p);
    if (numRead == -1)
        errExit("mq_receive");

    printf("Read %ld bytes; priority = %u\n", (long) numRead, prio);
    if (write(STDOUT_FILENO, buffer, numRead) == -1)
        errExit("write");
    write(STDOUT_FILENO, "\n", 1);

    exit(EXIT_SUCCESS);
}
