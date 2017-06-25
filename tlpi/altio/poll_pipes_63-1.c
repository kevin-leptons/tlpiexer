/*************************************************************************\
*                  Copyright (C) Michael Kerrisk, 2016.                   *
*                                                                         *
* This program is free software. You may use, modify, and redistribute it *
* under the terms of the GNU General Public License as published by the   *
* Free Software Foundation, either version 3 or (at your option) any      *
* later version. This program is distributed without any warranty.  See   *
* the file COPYING.gpl-v3 for details.                                    *
\*************************************************************************/

/* Listing 63-2 */

/* poll_pipes.c

   Example of the use of poll() to monitor multiple file descriptors.

   Usage: poll_pipes num-pipes [num-writes]
                                  def = 1

   Create 'num-pipes' pipes, and perform 'num-writes' writes to
   randomly selected pipes. Then use poll() to inspect the read ends
   of the pipes to see which pipes are readable.
*/
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>

#include "tlpi_hdr.h"

int
main(int argc, char *argv[])
{
    int num_pipes, i, ready, rand_pipe, num_writes;
    int (*pfds)[2];                     /* File descriptors for all pipes */
    fd_set rfd_set;
    int max_fd;

    if (argc < 2 || strcmp(argv[1], "--help") == 0)
        usageErr("%s num-pipes [num-writes]\n", argv[0]);

    // allocate memory for pipe descriptors 
    num_pipes = getInt(argv[1], GN_GT_0, "num-pipes");
    pfds = calloc(num_pipes, sizeof(int [2]));
    if (pfds == NULL)
        errExit("calloc");

    // create pipes
    for (i = 0; i < num_pipes; i++)
        if (pipe(pfds[i]) == -1)
            errExit("pipe %d", i);

    // random pipe to write
    num_writes = (argc > 2) ? getInt(argv[2], GN_GT_0, "num-writes") : 1;
    srandom((int) time(NULL));
    for (i = 0; i < num_writes; i++) {
        rand_pipe = random() % num_pipes;
        printf(
            "Writing to pipe: %i (%i:%i)\n", 
            rand_pipe + 1, pfds[rand_pipe][0], pfds[rand_pipe][1]
        );
        if (write(pfds[rand_pipe][1], "a", 1) == -1)
            errExit("write %d", pfds[rand_pipe][1]);
    }

    // build readable fd_set
    // detect max of file descriptors
    max_fd = 0;
    FD_ZERO(&rfd_set);
    for (i = 0; i < num_pipes; i++) {
        if (pfds[i][0] > FD_SETSIZE)
            errExit("File descriptor is limit under %i", FD_SETSIZE);
        FD_SET(pfds[i][0], &rfd_set);
        if (pfds[i][0] > max_fd)
            max_fd = pfds[i][0] + 1;
    }

    // wait to until pipes is readable
    ready = select(max_fd, &rfd_set, NULL, NULL, NULL);
    if (ready == -1)
        errExit("select");

    printf("select() returned: %d\n", ready);

    /* Check which pipes have data available for reading */

    for (i = 0; i < num_pipes; i++)
        if (FD_ISSET(pfds[i][0], &rfd_set))
            printf("Readable on fd: %3d\n", pfds[i][0]);

    exit(EXIT_SUCCESS);
}
