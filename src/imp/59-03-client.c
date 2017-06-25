/*
SPEC

    Part of solution for exercise 59-03. See 59-03.c for more information.

AUTHORS

    Kevin Leptons <kevin.leptons@gmail.com>
*/

#include <stdlib.h>
#include <stdio.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>

#include <tlpiexer/error.h>
#include <tlpiexer/59-03.h>

#define BUF_SIZE 1024

int main(int argc, char *argv[])
{
    int lfd;
    ssize_t tres;
    char buf[BUF_SIZE];

    lfd = usock_connect(USOCK_SERVER_PATH, SOCK_STREAM);
    if (lfd == FN_ER)
        exit_err("usock_connect");

    // read data from stdin into buffer
    // send it to peer until it's end
    for (;;) {
        tres = read(STDIN_FILENO, buf, sizeof(buf));
        if (tres == FN_ER)
            goto ERROR;
        if (tres == 0)
            break;
        if (write(lfd, buf, tres) != tres)
            goto ERROR;
    }

    close(lfd);
    return EXIT_SUCCESS;

ERROR:
    if (lfd > 0)
        close(lfd);
    return EXIT_FAILURE;
}
