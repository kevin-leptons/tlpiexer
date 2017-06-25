/*
SYNOPSIS

    Part of solution for excercise 57-03. See 57-03.c for more information.

AUTHORS

    Kevin Lepton <kevin.leptons@gmail.com>
*/

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <string.h>
#include <unistd.h>

#include <tlpiexer/error.h>
#include <tlpiexer/57-03.h>

static void _exit_usage(void);

int main(int argc, char *argv[])
{
    int lsock_fd;
    struct sockaddr_un lsock_addr;
    struct sockaddr_un psock_addr;
    struct request req;
    struct response res;
    int tres;

    // parse options
    if (argc != 2)
        _exit_usage();
    req.padding = atoi(argv[1]);
    if (req.padding == 0)
        _exit_usage();

    // setup local andress
    memset(&lsock_addr, 0, sizeof(lsock_addr));
    lsock_addr.sun_family = AF_UNIX;
    lsock_addr.sun_path[0] = 0;
    snprintf(lsock_addr.sun_path + 1, sizeof(lsock_addr.sun_path) - 2,
             "%s-%u", CLIENT_SOCK_PATH, getpid());

    // setup peer andress
    memset(&psock_addr, 0, sizeof(psock_addr));
    psock_addr.sun_family = AF_UNIX;
    psock_addr.sun_path[0] = 0;
    strncpy(psock_addr.sun_path + 1, SERVER_SOCK_PATH, 
            sizeof(psock_addr.sun_path) - 1);

    // create socket
    lsock_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (lsock_fd == FN_ER)
        exit_err("client:socket");

    // bind local socket to andress
    tres = bind(lsock_fd, (struct sockaddr*) &lsock_addr, 
                sizeof(lsock_addr));
    if (tres == FN_ER)
        exit_err("client:bind");

    // connect to peer socket
    tres = connect(lsock_fd, (struct sockaddr*) &psock_addr, 
                   sizeof(psock_addr));
    if (tres == FN_ER)
        exit_err("client:connect");

    // send request to add padding to sequence number
    if (write(lsock_fd, (char*) &req, sizeof(req)) != sizeof(req))
        exit_err("client:write");

    // receive response and display
    if (read(lsock_fd, (char*) &res, sizeof(res)) != sizeof(res))
        exit_err("client:read");
    printf("Response: %lu\n", res.seq_num);

    close(lsock_fd);
    return EXIT_SUCCESS;
}

static void _exit_usage(void)
{
    fprintf(stderr, "Usage: cmd <num-padding>\n");
    exit(EXIT_FAILURE);
}
