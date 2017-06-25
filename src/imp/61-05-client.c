
/*
SYNOPSIS

    Part of solution for exercise 61-05. See 61-05.c for more information.

AUTHORS

    Kevin Leptons <kevin.leptons@gmail.com>
*/

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>

#include <tlpiexer/error.h>
#include <tlpiexer/61-05.h>
#include <tlpiexer/59-04-isock.h>

int main(int argc, char *argv[])
{
    struct msg_req req;
    char buf[BUF_SIZE];
    int pfd;
    ssize_t rsize;

    // parse arguments
    if (argc != 5) {
        fprintf(stderr, "Use: %s <host> <usr> <pass> <cmd>\n", argv[0]);
        return EXIT_FAILURE;
    }

    // prepare request
    strncpy(req.usr, argv[2], sizeof(req.usr));
    strncpy(req.pwd, argv[3], sizeof(req.pwd));
    strncpy(req.cmd, argv[4], sizeof(req.cmd));

    // connect to server
    pfd = isock_connect(argv[1], SERVER_PORT, SOCK_STREAM);
    if (pfd == FN_ER)
        exit_err("isock_connect");

    // write request
    if (write(pfd, &req, sizeof(req)) != sizeof(req))
        exit_err("write");

    // read response and display
    for (;;) {
        rsize = read(pfd, buf, sizeof(buf));
        if (rsize == FN_ER)
            exit_err("read");
        if (rsize == 0)
            break;
        write(STDOUT_FILENO, buf, rsize);
    }

    // success  
    close(pfd);
    return EXIT_SUCCESS;
}
