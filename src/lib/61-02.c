#include <tlpiexer/61-02.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>

#include <tlpiexer/error.h>

#define SOCK_LISTEN_PATH_TMP "/tlpiexer-61-02"

int pipe_x(int pipefd[2])
{
    int l_fd = 0;
    int r_fd = 0;
    int w_fd = 0;
    struct sockaddr_un laddr;
    socklen_t laddr_len;
    int tres;

    // make listen socket
    memset(&laddr_len, 0, sizeof(laddr));
    laddr.sun_family = AF_UNIX;
    laddr.sun_path[0] = 0;
    snprintf(laddr.sun_path + 1, sizeof(laddr.sun_path) - 1, "%s-%i",
             SOCK_LISTEN_PATH_TMP, getpid());
    l_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (l_fd == FN_ER)
        goto ERROR;
    tres = bind(l_fd, (struct sockaddr*) &laddr, sizeof(laddr));
    if (tres == FN_ER)
        goto ERROR;
    if (listen(l_fd, 0) == FN_ER)
        goto ERROR;

    // make read socket, connect to listen socket
    r_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    tres = connect(r_fd, (struct sockaddr*) &laddr, sizeof(laddr));
    if (tres == FN_ER)
        goto ERROR;

    // listen socket accept connection from read socket
    w_fd = accept(l_fd, NULL, NULL);
    if (w_fd == FN_ER)
        goto ERROR;
    close(l_fd);

    // make connection to one-way
    if (shutdown(r_fd, SHUT_WR) == FN_ER)
        goto ERROR;
    if (shutdown(w_fd, SHUT_RD) == FN_ER)
        goto ERROR;

    // success
    pipefd[0]  = r_fd;
    pipefd[1] = w_fd;
    return FN_OK;

ERROR:
    if (l_fd > 0)
        close(l_fd);
    if (r_fd > 0)
        close(r_fd);
    if (w_fd > 0)
        close(w_fd);
    return FN_ER;
}
