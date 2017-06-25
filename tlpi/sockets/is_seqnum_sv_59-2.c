/*************************************************************************\
*                  Copyright (C) Michael Kerrisk, 2016.                   *
*                                                                         *
* This program is free software. You may use, modify, and redistribute it *
* under the terms of the GNU General Public License as published by the   *
* Free Software Foundation, either version 3 or (at your option) any      *
* later version. This program is distributed without any warranty.  See   *
* the file COPYING.gpl-v3 for details.                                    *
\*************************************************************************/

/* Listing 59-6 */

/* is_seqnum_sv.c

   A simple Internet stream socket server. Our service is to provide
   unique sequence numbers to clients.

   Usage:  is_seqnum_sv [init-seq-num]
                        (default = 0)

   See also is_seqnum_cl.c.
*/
#define _BSD_SOURCE             /* To get definitions of NI_MAXHOST and
                                   NI_MAXSERV from <netdb.h> */
#include <netdb.h>
#include "is_seqnum.h"
#include "inet_sockets.h"

#define BACKLOG 50
#define ADDRSTRLEN (NI_MAXHOST + NI_MAXSERV + 10)

int
main(int argc, char *argv[])
{
    uint32_t seqNum;
    char reqLenStr[INT_LEN];            /* Length of requested sequence */
    char seqNumStr[INT_LEN];            /* Start of granted sequence */
    struct sockaddr_storage claddr;
    int lfd, cfd, reqLen;
    socklen_t addrlen;
    char addrStr[ADDRSTRLEN];
    char host[NI_MAXHOST];
    char service[NI_MAXSERV];

    if (argc > 1 && strcmp(argv[1], "--help") == 0)
        usageErr("%s [init-seq-num]\n", argv[0]);

    seqNum = (argc > 1) ? getInt(argv[1], 0, "init-seq-num") : 0;

    // Ignore the SIGPIPE signal, so that we find out about broken connection
    // errors via a failure from write().

    if (signal(SIGPIPE, SIG_IGN) == SIG_ERR)
        errExit("signal");

    // listen
    lfd = inetListen(PORT_NUM, BACKLOG, NULL);

    // handle request
    for (;;) {      

        /* Accept a client connection, obtaining client's address */

        addrlen = sizeof(struct sockaddr_storage);
        cfd = accept(lfd, (struct sockaddr *) &claddr, &addrlen);
        if (cfd == -1) {
            errMsg("accept");
            continue;
        }

        if (getnameinfo((struct sockaddr *) &claddr, addrlen,
                    host, NI_MAXHOST, service, NI_MAXSERV, 0) == 0)
            snprintf(addrStr, ADDRSTRLEN, "(%s, %s)", host, service);
        else
            snprintf(addrStr, ADDRSTRLEN, "(?UNKNOWN?)");
        printf("Connection from %s\n", addrStr);

        /* Read client request, send sequence number back */

        if (readLine(cfd, reqLenStr, INT_LEN) <= 0) {
            close(cfd);
            continue;                   /* Failed read; skip request */
        }

        reqLen = atoi(reqLenStr);
        if (reqLen <= 0) {              /* Watch for misbehaving clients */
            close(cfd);
            continue;                   /* Bad request; skip it */
        }

        snprintf(seqNumStr, INT_LEN, "%d\n", seqNum);
        if (write(cfd, seqNumStr, strlen(seqNumStr)) != strlen(seqNumStr))
            fprintf(stderr, "Error on write");

        seqNum += reqLen;               /* Update sequence number */

        if (close(cfd) == -1)           /* Close connection */
            errMsg("close");
    }
}
