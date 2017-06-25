/*
SYNOPSIS

    Solution for exercise 61-04.

REQUIREMENT

    Write a program that uses getsockname() to show that, if we call listen()
    on a TCP socket without first calling bind(), the socket is assigned an
    ephemeral port number.

SOLUTION

    Repeat operation below:

        - Create socket.
        - Call listen on that socket.
        - Call getsockname() to retrieve port of that socket.
        - Write port number to stdout

    Each time, printed port numbers are different. It mean that port numbers
    are selected by kernel - ephemeral port number.

USAGE

    $ ./dest/bin/61-04
    38069
    56762
    14280

AUTHORS

    Kevin Leptons <kevin.leptons@gmail.com>
*/

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#include <tlpiexer/error.h>

#define BACKLOG 8
#define REPEAT_TIMES 3

int main(int argc, char *argv[])
{
    int lfd;
    size_t i;
    struct sockaddr_in addr;
    socklen_t addr_len;

    for (i = 0; i < REPEAT_TIMES; i++) {
        lfd = socket(AF_INET, SOCK_STREAM, 0);
        if (lfd == FN_ER)
            exit_err("socket");
        if (listen(lfd, BACKLOG) == FN_ER)
            exit_err("listen");

        addr_len = sizeof(addr_len);
        if (getsockname(lfd, (struct sockaddr*) &addr, &addr_len) == FN_ER)
            exit_err("getsockname");
        printf("%i\n", addr.sin_port);

        close(lfd);
    }

    return EXIT_SUCCESS;
}
