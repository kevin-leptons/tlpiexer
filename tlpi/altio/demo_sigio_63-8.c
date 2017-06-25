/*************************************************************************\
*                  Copyright (C) Michael Kerrisk, 2016.                   *
*                                                                         *
* This program is free software. You may use, modify, and redistribute it *
* under the terms of the GNU General Public License as published by the   *
* Free Software Foundation, either version 3 or (at your option) any      *
* later version. This program is distributed without any warranty.  See   *
* the file COPYING.gpl-v3 for details.                                    *
\*************************************************************************/

/* Listing 63-3 */

/* demo_sigio.c

   A trivial example of the use of signal-driven I/O.
*/

#define _GNU_SOURCE

#include <signal.h>
#include <ctype.h>
#include <fcntl.h>
#include <termios.h>
#include "tty_functions.h"      /* Declaration of ttySetCbreak() */
#include "tlpi_hdr.h"

static volatile sig_atomic_t gotSigio = 0;
static siginfo_t *siginfo;
                                /* Set nonzero on receipt of SIGIO */
#define SIG_RTIO SIGRTMIN + 3

static void
sigio_handler(int sig, siginfo_t *si, void *uctx)
{
    gotSigio = 1;
    siginfo = si;
}

int
main(int argc, char *argv[])
{
    int flags, j, cnt;
    struct termios origTermios;
    char ch;
    struct sigaction sa;
    Boolean done;

    // establish I/O signal
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART | SA_SIGINFO;
    sa.sa_sigaction = sigio_handler;
    if (sigaction(SIG_RTIO, &sa, NULL) == -1)
        errExit("sigaction");

    // put current process to owner of I/O signal
    if (fcntl(STDIN_FILENO, F_SETOWN, getpid()) == -1)
        errExit("fcntl(F_SETOWN)");

    // enable I/0 is noblocking
    flags = fcntl(STDIN_FILENO, F_GETFL);
    if (fcntl(STDIN_FILENO, F_SETFL, flags | O_ASYNC | O_NONBLOCK) == -1)
        errExit("fcntl(F_SETFL)");

    // use real time I/O signal
    if (fcntl(STDIN_FILENO, F_SETSIG, SIG_RTIO) != 0)
        errExit("fcntl:F_SETSIG");

    /* Place terminal in cbreak mode */

    if (ttySetCbreak(STDIN_FILENO, &origTermios) == -1)
        errExit("ttySetCbreak");

    for (done = FALSE, cnt = 0; !done ; cnt++) {
        for (j = 0; j < 100000000; j++)
            continue;                   /* Slow main loop down a little */

        if (gotSigio) {                 /* Is input available? */
            gotSigio = 0;

            /* Read all available input until error (probably EAGAIN)
               or EOF (not actually possible in cbreak mode) or a
               hash (#) character is read */

            while (read(STDIN_FILENO, &ch, 1) > 0 && !done) {
                printf("cnt=%d; read %c\n", cnt, ch);
                printf("... => signal: signo=%i: si_fd=%i, si_code=%i\n", 
                       siginfo->si_signo, siginfo->si_fd, siginfo->si_code);

                done = ch == '#';
            }
        }
    }

    /* Restore original terminal settings */

    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &origTermios) == -1)
        errExit("tcsetattr");
    exit(EXIT_SUCCESS);
}
