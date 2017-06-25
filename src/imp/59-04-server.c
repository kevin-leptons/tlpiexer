/*
SYNOPSIS

    Part of solution for excercise 59-04. See 59-04.c for more information.

AUTHORS

    Kevin Leptons <kevin.leptons@gmail.com>
*/

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <signal.h>
#include <netinet/in.h>

#include <tlpiexer/error.h>
#include <tlpiexer/59-04.h>
#include <tlpiexer/59-04-llist.h>
#include <tlpiexer/59-04-isock.h>

static int lfd = 0;
static int pfd = 0;

static void term_handle(int sig);

int main(int argc, char *argv[])
{
    struct sockaddr_storage caddr;
    socklen_t caddr_len;
    struct msg_req req;
    struct msg_res res;
    struct llist list;

    if (signal(SIGPIPE, SIG_IGN) == SIG_ERR)
        exit_err("signal:SIGPIPE");
    if (signal(SIGTERM, term_handle) == SIG_ERR)
        exit_err("signal:SIGTERM");
    if (signal(SIGINT, term_handle) == SIG_ERR)
        exit_err("signal:SIGINT");

    // init storage for name-value items
    ll_init(&list);

    // listen on service
    lfd = isock_listen(SERVER_PORT, SOCK_STREAM, BACKLOG);
    if (lfd == FN_ER)
        exit_err("isock_listen");

    // handle peer request
    for (;;) {
        memset(&caddr, 0, sizeof(caddr));
        caddr_len = sizeof(caddr);
        pfd = accept(lfd, (struct sockaddr*) &caddr, &caddr_len);
        if (pfd == FN_ER)
            continue;

        // read request
        if (read(pfd, &req, sizeof(req)) != sizeof(req)) {
            close(pfd);
            continue;
        }

        // chose suitable command
        lerrno = 0;
        if (req.cmd == REQ_ADD)
            ll_add(&list, &req.item);
        else if (req.cmd == REQ_GET) {
            memcpy(&res.item.name, &req.item.name, sizeof(res.item.name));
            ll_get(&list, &res.item);
        }
        else if (req.cmd == REQ_SET)
            ll_set(&list, &req.item);
        else if (req.cmd == REQ_DEL)
            ll_del(&list, &req.item);
        else 
            lerrno = RES_UNKOW;

        // write response
        if (lerrno == 0)
            res.result = RES_OK;
        else
            res.result = lerrno;
        if (write(pfd, &res, sizeof(res)) != sizeof(res)) {
            close(pfd);
            continue;
        }

        // done
        close(pfd);
        pfd = 0;
    }
}

void term_handle(int sig)
{
    if (lfd > 0)
        close(lfd);
    if (pfd > 0)
        close(pfd);
    signal(sig, SIG_DFL);
    kill(getpid(), sig);
}
