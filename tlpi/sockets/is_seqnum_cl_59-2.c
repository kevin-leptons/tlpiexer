/*************************************************************************\
*                  Copyright (C) Michael Kerrisk, 2016.                   *
*                                                                         *
* This program is free software. You may use, modify, and redistribute it *
* under the terms of the GNU General Public License as published by the   *
* Free Software Foundation, either version 3 or (at your option) any      *
* later version. This program is distributed without any warranty.  See   *
* the file COPYING.gpl-v3 for details.                                    *
\*************************************************************************/

/* Listing 59-7 */

/* is_seqnum_cl.c

   A simple Internet stream socket client. This client requests a sequence
   number from the server.

   See also is_seqnum_sv.c.
*/
#include <netdb.h>
#include "is_seqnum.h"
#include "inet_sockets.h"

int
main(int argc, char *argv[])
{
    char *reqLenStr;                    /* Requested length of sequence */
    char seqNumStr[INT_LEN];            /* Start of granted sequence */
    int cfd;
    ssize_t numRead;

    if (argc < 2 || strcmp(argv[1], "--help") == 0)
        usageErr("%s server-host [sequence-len]\n", argv[0]);

    // connect
    cfd = inetConnect(argv[1], PORT_NUM, SOCK_STREAM) ;
    if (cfd <= 0)
        errExit("inetConnect");

    /* Send requested sequence length, with terminating newline */

    reqLenStr = (argc > 2) ? argv[2] : "1";
    if (write(cfd, reqLenStr, strlen(reqLenStr)) !=  strlen(reqLenStr))
        fatal("Partial/failed write (reqLenStr)");
    if (write(cfd, "\n", 1) != 1)
        fatal("Partial/failed write (newline)");

    /* Read and display sequence number returned by server */

    numRead = readLine(cfd, seqNumStr, INT_LEN);
    if (numRead == -1)
        errExit("readLine");
    if (numRead == 0)
        fatal("Unexpected EOF from server");

    printf("Sequence number: %s", seqNumStr);   /* Includes '\n' */

    exit(EXIT_SUCCESS);                         /* Closes 'cfd' */
}
