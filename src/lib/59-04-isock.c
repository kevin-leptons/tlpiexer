#include <tlpiexer/59-04-isock.h>

#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>

#include <tlpiexer/error.h>

#define AUTO_ADDR "0.0.0.0"

int isock_connect(const char *host, const char *service, int type)
{
    int lfd;
    struct addrinfo *addrs;
    struct addrinfo *addr;
    struct addrinfo hints;
    ssize_t tres;

    // search all of address suitable
    memset(&hints, 0, sizeof(hints));
    hints.ai_canonname = NULL;
    hints.ai_addr = NULL;
    hints.ai_next = NULL;
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = type;
    hints.ai_flags = AI_NUMERICSERV;
    tres = getaddrinfo(host, service, &hints, &addrs);
    if (tres != FN_OK)
        return tres;

    // try to connect with each address
    // pick up first success connection
    for (addr = addrs; addr != NULL; addr = addr->ai_next) {
        lfd = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);
        if (lfd == FN_ER)
            continue;
        tres = connect(lfd, addr->ai_addr, addr->ai_addrlen);
        if (tres == FN_ER) {
            close(lfd);
            continue;
        }
        break;
    }
    freeaddrinfo(addrs);

    // not suitable address
    if (addr == NULL)
        return FN_ER;

    // success
    return lfd;
}

int isock_listen(const char *service, int type, int backlog)
{
    struct addrinfo *addrs;
    struct addrinfo *addr;
    struct addrinfo hints;
    int lfd;
    ssize_t tres;
    int optv = 1;

    // search all andress suitable
    memset(&hints, 0, sizeof(hints));
    hints.ai_canonname = NULL;
    hints.ai_addr = NULL;
    hints.ai_next = NULL;
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = type;
    tres = getaddrinfo(AUTO_ADDR, service, &hints, &addrs);
    if (tres != FN_OK)
        return tres;

    // try to create, bind socket with each address
    // pick up first success
    for (addr = addrs; addr != NULL; addr = addr->ai_next) {
        lfd = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);
        if (lfd == FN_ER)
            continue;
        tres = setsockopt(lfd, SOL_SOCKET, SO_REUSEADDR, &optv, sizeof(optv));
        if (tres == FN_ER) {
            close(lfd);
            continue;
        }
        tres = bind(lfd, addr->ai_addr, addr->ai_addrlen);
        if (tres == FN_ER) {
            close(lfd);
            continue;
        }
        if (listen(lfd, backlog) == FN_ER) {
            close(lfd);
            continue;
        }
        break;
    }
    freeaddrinfo(addrs);

    // doesn't find suitable address
    if (addr == NULL)
        return FN_ER;

    // success
    return lfd;
}

int isock_bind(int fd, const char *service, int type)
{
    struct addrinfo *addrs;
    struct addrinfo *addr;
    struct addrinfo hints;
    int tres;

    // search all suitable socket
    memset(&hints, 0, sizeof(hints));
    hints.ai_canonname = NULL;
    hints.ai_addr = NULL;
    hints.ai_next = NULL;
    hints.ai_socktype = type;
    hints.ai_family = AF_UNSPEC;
    hints.ai_flags = AI_NUMERICSERV;
    tres = getaddrinfo(AUTO_ADDR, service, &hints, &addrs);
    if (tres != FN_OK)
        return tres;

    // try to bind socket
    // pick up first success
    for (addr = addrs; addr != NULL; addr = addr->ai_next) {
        if (bind(fd, addr->ai_addr, addr->ai_addrlen) == FN_ER)
            continue;
        break;
    }
    freeaddrinfo(addrs);

    // not bind
    if (addr == NULL)
        return FN_ER;

    // success
    errno = 0;
    return FN_OK;
}
