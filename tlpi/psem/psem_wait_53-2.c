/*************************************************************************\
*                  Copyright (C) Michael Kerrisk, 2016.                   *
*                                                                         *
* This program is free software. You may use, modify, and redistribute it *
* under the terms of the GNU General Public License as published by the   *
* Free Software Foundation, either version 3 or (at your option) any      *
* later version. This program is distributed without any warranty.  See   *
* the file COPYING.gpl-v3 for details.                                    *
\*************************************************************************/

/* Listing 53-3 */

/* psem_wait.c

   Decrease the value of a POSIX named semaphore.

   See also psem_post.c.

   On Linux, named semaphores are supported with kernel 2.6 or later, and
   a glibc that provides the NPTL threading implementation.
*/
#include <semaphore.h>
#include <time.h>
#include "tlpi_hdr.h"

void exit_usage(void);

int
main(int argc, char *argv[])
{
    sem_t *sem;
    int timeout;
    int opt;
    struct timespec abs_time;
    char *sem_name;

    if (argc < 2 || strcmp(argv[1], "--help") == 0)
        usageErr("%s sem-name\n", argv[0]);

    for (;;) {
        opt = getopt(argc, argv, "t:");
        if (opt == -1)
            break;
        if (opt == 't')
            timeout = atoi(optarg);
        else
            exit_usage();
    }

    if (optind > argc)
        exit_usage();
    sem_name = argv[optind];

    // make absolute time from now and relative time
    if (clock_gettime(CLOCK_REALTIME, &abs_time) == -1)
        errExit("clock_gettime");
    abs_time.tv_sec += timeout;

    sem = sem_open(sem_name, 0);
    if (sem == SEM_FAILED)
        errExit("sem_open");

    errno = 0;
    if (sem_timedwait(sem, &abs_time) == -1)
        errExit("sem_timedwait");
    if (errno == ETIMEDOUT)
        errExit("timeout");

    printf("%ld sem_timedwait() succeeded\n", (long) getpid());
    exit(EXIT_SUCCESS);
}

void exit_usage(void)
{
    fprintf(stderr, "USAGE: <cmd> -t <wait-time> <sem-name>");
    exit(EXIT_FAILURE);
}
