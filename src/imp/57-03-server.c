/*
SYNOPSIS

    Part of solution for exercise 57-03. See 57-03.c for more information.

AUTHORS

    Kevin Leptons <kevin.leptons@gmail.com>
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

int main(int argc, char *argv[])
{
    size_t seq_num = 0;
    int lsock_fd;
    int psock_fd;
    struct sockaddr_un lsock_addr;
    struct sockaddr_un psock_addr;
    socklen_t psock_len;
    struct request req;
    struct response res;
    int tres;

    // create local socket
    lsock_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (lsock_fd == FN_ER)
        exit_err("server:socket");

    // setup local andress
    memset(&lsock_addr, 0, sizeof(lsock_addr));
    lsock_addr.sun_family = AF_UNIX;
    lsock_addr.sun_path[0] = 0;
    strncpy(lsock_addr.sun_path + 1, SERVER_SOCK_PATH, 
            sizeof(lsock_addr.sun_path) - 2);

    // setup peer andress
    psock_len = sizeof(psock_addr);

    // bind socket to address
    tres = bind(lsock_fd, (struct sockaddr*) &lsock_addr, sizeof(lsock_addr));
    if (tres == FN_ER)
        exit_err("server:bind");
    
    // listen connection comming
    if (listen(lsock_fd, SOCK_BACKLOG) == FN_ER)
        exit_err("server:listen");

    // enter loop to accept connection
    // process request and send back response
    for (;;) {
        psock_fd = accept(lsock_fd, 
                          (struct sockaddr*) &psock_addr, &psock_len);
        if (psock_fd == FN_ER)
            continue;
        if (read(psock_fd, (char*) &req, sizeof(req)) != sizeof(req)) {
            msg_err("server:read");
            close(psock_fd);
            continue;
        }
        seq_num += req.padding;
        res.seq_num = seq_num;
        if (write(psock_fd, (char*) &res, sizeof(res)) != sizeof(res)) {
            msg_err("server:write");
            close(psock_fd);
            continue;
        }
        close(psock_fd);
    }

    return EXIT_SUCCESS;
}
