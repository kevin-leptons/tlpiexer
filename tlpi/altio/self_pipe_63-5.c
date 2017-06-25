/*************************************************************************\
*                  Copyright (C) Michael Kerrisk, 2016.                   *
*                                                                         *
* This program is free software. You may use, modify, and redistribute it *
* under the terms of the GNU General Public License as published by the   *
* Free Software Foundation, either version 3 or (at your option) any      *
* later version. This program is distributed without any warranty.  See   *
* the file COPYING.gpl-v3 for details.                                    *
\*************************************************************************/

/* Listing 63-9 */

/*  self_pipe.c

   Employ the self-pipe trick so that we can avoid race conditions while both
   selecting on a set of file descriptors and also waiting for a signal.

   Usage as shown in synopsis below; for example:

        self_pipe - 0
*/
#include <sys/time.h>
#include <fcntl.h>
#include <signal.h>
#include <poll.h>
#include <string.h>
#include "tlpi_hdr.h"

#define POLLFD_NUM 2
#define MONITOR_SIG SIGUSR1

struct pipe_msg
{
    int signo;
};

static int pfd[2];                      /* File descriptors for pipe */

static void
handler(int sig)
{
    int savedErrno;                     /* In case we change 'errno' */
    struct pipe_msg pmsg;
    ssize_t wsize;

    savedErrno = errno;
    pmsg.signo = sig;

    wsize = write(pfd[1], &pmsg, sizeof(pmsg));
    if (wsize < 0 && errno != EAGAIN)
        errExit("write");

    errno = savedErrno;
}

int
main(int argc, char *argv[])
{
    int ready, flags;
    struct sigaction sa;
    struct pollfd poll_fd[POLLFD_NUM];
    int timeout;
    char buf[1024];
    ssize_t rsize;
    struct pipe_msg pmsg;

    if (argc < 2 || strcmp(argv[1], "--help") == 0)
        usageErr("%s {timeout|-} fd...\n"
                "\t\t('-' means infinite timeout)\n", argv[0]);

    // create pipe
    if (pipe(pfd) == -1)
        errExit("pipe");

    // make pipe read-end is nonblocking
    flags = fcntl(pfd[0], F_GETFL);
    if (flags == -1)
        errExit("fcntl-F_GETFL");
    flags |= O_NONBLOCK;
    if (fcntl(pfd[0], F_SETFL, flags) == -1)
        errExit("fcntl-F_SETFL");

    // make pipe write-end is nonblocking
    flags = fcntl(pfd[1], F_GETFL);
    if (flags == -1)
        errExit("fcntl-F_GETFL");
    flags |= O_NONBLOCK;
    if (fcntl(pfd[1], F_SETFL, flags) == -1)
        errExit("fcntl-F_SETFL");

    // establish signal handler
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    sa.sa_handler = handler;
    if (sigaction(MONITOR_SIG, &sa, NULL) == -1)
        errExit("sigaction");

    // initialize data for poll()
    if (strcmp(argv[1], "-") == 0) 
        timeout = -1;
    else
        timeout = atoi(argv[1]);
    poll_fd[0].fd = STDIN_FILENO;
    poll_fd[0].events = POLLIN;
    poll_fd[1].fd = pfd[0];
    poll_fd[1].events = POLLIN;

    // monitor
    for (;;) {
        ready = poll(poll_fd, POLLFD_NUM, timeout);
        if (ready < 0) {
            if (errno == EINTR)
                continue;
            else
                errExit("poll");
        }

        // inspect stdin
        if (poll_fd[0].revents & POLLIN) {
            rsize = read(STDIN_FILENO, buf, sizeof(buf));
            if (rsize < 0)
                errExit("read:stdin");
            buf[rsize] = 0;
            printf("... => stdin: %s\n", buf);
        }
        
        // inspect pipe
        if (poll_fd[1].revents & POLLIN) {
            for (;;) {
                rsize = read(pfd[0], &pmsg, sizeof(pmsg));
                if (rsize < 0) {
                    if (errno == EAGAIN)
                        break;
                }
                if (rsize != sizeof(pmsg))
                    errExit("read:poll_fd:read_end");

                printf("... => signal: %i - %s\n", 
                       pmsg.signo, strsignal(pmsg.signo));
            }
        }
    }

    // done
    return EXIT_SUCCESS;
}
