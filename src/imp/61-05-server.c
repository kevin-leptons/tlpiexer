/*
SYNOPSIS

    Part of solution of exercise 61-05. See 61-05.c for more information.

AUTHORS

    Kevin Leptons <kevin.leptosn@gmail.com>
*/

#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include <tlpiexer/error.h>
#include <tlpiexer/61-05.h>
#include <tlpiexer/59-04-isock.h>

static int lfd;
static int pfd;
static pid_t pid;

static void parent_thanle(int sig);
static void child_thanle(int sig);
static void parent_chandle(int sig);
static void req_handle(int pfd);

int main(int argc, char *argv[])
{
    // setup terminate signal for parent
    if (signal(SIGTERM, parent_thanle) == SIG_ERR)
        exit_err("parent:signal:SIGTERM");
    if (signal(SIGINT, parent_thanle) == SIG_ERR)
        exit_err("parent:signal:SIGINT");
    if (signal(SIGCHLD, parent_chandle) == SIG_ERR)
        exit_err("parent:signal:SIGCHLD");

    // listen on socket
    lfd = isock_listen(SERVER_PORT, SOCK_STREAM, BACKLOG);
    if (lfd == FN_ER)
        return FN_ER;

    // concurent request handler
    for (;;) {
        pfd = accept(lfd, NULL, NULL);
        if (pfd == FN_ER)
            continue;
        pid = fork();
        if (pid == FN_ER)
            kill(getpid(), SIGTERM);
        else if (pid == 0)
            req_handle(pfd);
        else
            close(pfd);
    }
}

static void req_handle(int pfd)
{
    struct msg_req req;
    pid_t pid;

    // reset child terminate signal handler
    if (signal(SIGTERM, child_thanle) == SIG_ERR)
        goto ERROR;
    if (signal(SIGINT, SIG_DFL) == SIG_ERR)
        goto ERROR;

    // read request
    if (read(pfd, &req, sizeof(req)) != sizeof(req)) 
        goto ERROR;
    req.cmd[sizeof(req.cmd) - 1] = 0;

    // sign in as a user
    if (signin(req.usr, req.pwd) == FN_ER)
        goto ERROR;

    // redirect stdout, stderr to client
    dup2(pfd, STDOUT_FILENO);
    dup2(pfd, STDERR_FILENO);

    // create grant child process to perform command
    pid = fork();
    if (pid == FN_ER) {
        goto ERROR;
    }
    else if (pid == 0) {
        execl("/bin/sh", "sh", "-c", req.cmd, (char*) 0);
        exit_err("grant-child:execl");
    }

    // child process fall down here
    waitpid(-1, NULL, WNOHANG);
    close(pfd);
    exit(EXIT_SUCCESS);
ERROR:
    close(pfd);
    fprintf(stderr, "child:error:%s\n", strerror(errno));
    exit(EXIT_FAILURE);
}

static void parent_thanle(int sig)
{
    // kill all of children
    // ?
    
    // free parent
    if (lfd > 0)
        close(lfd);

    // resend signal
    signal(sig, SIG_DFL);
    kill(getpid(), sig);
}

static void parent_chandle(int sig)
{
    int terrno = errno;
    
    for (;;) {
        if (waitpid(-1, NULL, WNOHANG) <= 0)
            break;
    }

    errno = terrno;
}

static void child_thanle(int sig)
{
    close(pfd);
    signal(sig, SIG_DFL);
    kill(getpid(), sig);
}
