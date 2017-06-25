#include <tlpiexer/59-03.h>

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <sys/un.h>
#include <sys/socket.h>

#include <tlpiexer/error.h>

#define LOCAL_PATH_TMP "/tlpiexer-59-03-client"

int usock_connect(const char *host, int type)
{
    int lfd;
    struct sockaddr_un laddr;
    struct sockaddr_un paddr;

    // prepare local socket address
    memset(&laddr, 0, sizeof(laddr));
    laddr.sun_family = AF_UNIX;
    laddr.sun_path[0] = 0;
    snprintf(laddr.sun_path + 1, sizeof(laddr) - 1, "%s-%u",
             LOCAL_PATH_TMP, getpid());

    // prepare peer socket address
    memset(&paddr, 0, sizeof(paddr));
    paddr.sun_family = AF_UNIX;
    paddr.sun_path[0] = 0;
    strncpy(paddr.sun_path + 1, host, sizeof(paddr.sun_path) - 1);

    // create, bind local socket
    lfd = socket(AF_UNIX, type, 0);
    if (lfd == FN_ER)
        return FN_ER;
    if (bind(lfd, (struct sockaddr*) &laddr, sizeof(laddr)) == FN_ER)
        goto ERROR;

    // connect local to peer socket
    if (connect(lfd, (struct sockaddr*) &paddr, sizeof(paddr)) == FN_ER)
        goto ERROR;

    // success
    return lfd;

ERROR:
    if (lfd > 0)
        close(lfd);
    return FN_ER;
}

int usock_listen(const char *host, int type, int backlog)
{
    int lfd;
    struct sockaddr_un laddr;

    // prepare local address
    memset(&laddr, 0, sizeof(laddr));
    laddr.sun_family = AF_UNIX;
    laddr.sun_path[0] = 0;
    strncpy(laddr.sun_path + 1, host, sizeof(laddr.sun_path) - 1);

    // create, bind local socket
    lfd = socket(AF_UNIX, type, 0);
    if (lfd == FN_ER)
        return FN_ER;
    if (bind(lfd, (struct sockaddr*) &laddr, sizeof(laddr)) == FN_ER)
        goto ERROR;

    // mark listen on socket
    if (listen(lfd, backlog) == FN_ER)
        goto ERROR;

    // success
    return lfd;

ERROR:
    if (lfd > 0)
        close(lfd);
    return FN_ER;
}
