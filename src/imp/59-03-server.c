/*
SYNOPSIS

    Part of solution for excercise 59-03. See 59-03.c for more information.

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

static int lfd = 0;
static int pfd = 0;

static void term_handle(int sig);
static void free_res(void);

int main(int argc, char *argv[])
{
    struct sockaddr_un paddr;
    socklen_t paddr_len;
    ssize_t tsize;
    char buf[BUF_SIZE];

    if (signal(SIGTERM, term_handle) == SIG_ERR)
        exit_err("signal:SIGTERM");
    if (signal(SIGINT, term_handle) == SIG_ERR)
        exit_err("signal:SIGINT");
    if (signal(SIGPIPE, SIG_IGN) == SIG_ERR)
        exit_err("signal:SIG_ING");

    lfd = usock_listen(USOCK_SERVER_PATH, SOCK_STREAM, BACKLOG);
    if (lfd == FN_ER)
        exit_err("usock_listen");

    // hanle client request
    paddr_len = sizeof(paddr);
    memset(&paddr, 0, sizeof(paddr));
    for (;;) {
        pfd = accept(lfd, (struct sockaddr*) &paddr, &paddr_len);
        if (pfd == FN_ER)
            continue;

        // read data from peer until it's end
        // write it's data to stdout
        for (;;) {
            tsize = read(pfd, buf, sizeof(buf));
            if (tsize == FN_ER) {
                msg_err("read:pfd");
                break;
            }
            if (tsize == 0)
                break;
            if (write(STDOUT_FILENO, buf, tsize) != tsize) {
                msg_err("write:stdout");
                break;
            }
        }

        close(pfd);
    }
}

static void term_handle(int sig)
{
    free_res();
    signal(sig, SIG_DFL);
    kill(getpid(), sig);
}

static void free_res(void)
{
    if (lfd > 0)
        close(lfd);
    if (pfd > 0)
        close(pfd);
}
