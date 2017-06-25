/*
SYNOPSIS

    Part of solution for excercise 59-01. See 59-01.c to get more information.

AUTHORS

    Kevin Leptons <kevin.leptons@gmail.com>
*/

#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>

#include <tlpiexer/error.h>
#include <tlpiexer/59-01.h>

int main(int argc, char *argv[])
{
    char line[INT_LEN];
    ssize_t tres;
    size_t tsize;
    struct addrinfo hints;
    struct addrinfo *addr;
    struct addrinfo *a;
    int lfd;
    char *num;
    struct lbuf lbuf;

    if (argc < 2 || argc > 3) {
        fprintf(stderr, "Use: %s <server-addr> [number]\n", argv[0]);
        return EXIT_FAILURE;
    }

    // search server address
    memset(&hints, 0, sizeof(hints));
    hints.ai_canonname = NULL;
    hints.ai_addr = NULL;
    hints.ai_next = NULL;
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_NUMERICSERV;
    if (getaddrinfo(argv[1], PORT_NUM, &hints, &addr) != FN_OK)
        exit_err("client:getaddrinfo");

    // try connect to server
    for (a = addr; a != NULL; a = a->ai_next) {
        lfd = socket(a->ai_family, a->ai_socktype, a->ai_protocol);
        if (lfd == FN_ER)
            continue;
        if (connect(lfd, a->ai_addr, a->ai_addrlen) == FN_ER) {
            close(lfd);
            continue;
        }
        break;
    }
    if (a == NULL)
        exit_err("client:search server");
    freeaddrinfo(addr);

    // process input params
    if (argc == 3) {
        if (atoi(argv[2]) == 0)
            goto ERROR;
        num = argv[2];
    } else {
        num = "1";
    }
    snprintf(line, sizeof(line), "%s\n", num);

    // send request
    tsize = strlen(line);
    tres = write(lfd, line, tsize);
    if (tres != tsize)
        goto ERROR;
    
    // receive response
    init_lbuf(lfd, &lbuf);
    tres = read_lbuf(&lbuf, line, sizeof(line));
    if (tres <= 0)
        goto ERROR;

    // display response
    printf("client:response:%s\n", line);
    
    close(lfd);
    return EXIT_SUCCESS;

ERROR:
    if (lfd > 0)
        close(lfd);
    fprintf(stderr, "client:%s\n", strerror(errno));
    return EXIT_FAILURE;
}
