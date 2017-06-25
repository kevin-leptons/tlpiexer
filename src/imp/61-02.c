/*
SPEC

    Solution for exercise 61-02.

REQUIREMENT

    Implement pipe() in terms of socketpair(). Use shutdown() to ensure that
    the resulting pipe is unidirectional.

SOLUTION

    Create new interface as: pipe_x(int pfd[2]). It is implement follow:

    Create sockets:

        - sfd_l: listen socket
        - sfd_r: read socket, corresponse with read end pipe

    Connect sfd_r to sfd_l. Accept on sfd_l make sfd_w, corresponse with
    write end pipe.

    Close sfd_l.

    Disable sfd_r writing and sfd_w reading.

    Return sfd_r, sfd_w as read and write pipes.

    Interface and implement put in

        - include/tlpiexer/61-2.h
        - src/lib/61-2.c

    Main program try to use it.

USAGE

    $ ./dest/bin/61-02
    child:write:hello there
    parent:read:hello there

AUTHORS

    Kevin Leptons <kevin.leptons@gmail.com>
*/

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include <tlpiexer/error.h>
#include <tlpiexer/61-02.h>

#define PIPE_MSG "hello there"

int main(int argc, char *argv[])
{
    int pfds[2] = {0, 0};
    pid_t tpid;
    char buf[sizeof(PIPE_MSG) + 1];
    ssize_t tres;

    // disable user buffer for correct ouput
    setbuf(stdout, NULL);
    setbuf(stderr, NULL);

    if (pipe_x(pfds) == FN_ER)
        exit_err("pipe_x");
    tpid = fork();
    if (tpid == FN_ER) {
        exit_err("fork");
    }
    else if (tpid == 0) {
        // child process, write message to parent
        close(pfds[0]);
        tres = write(pfds[1], PIPE_MSG, sizeof(PIPE_MSG));
        close(pfds[1]);
        if (tres != sizeof(PIPE_MSG))
            exit_err("child:write");
        printf("child:write:%s\n", PIPE_MSG);
        exit(EXIT_SUCCESS);
    } else {
        // parent process, read message from child
        close(pfds[1]);
        tres = read(pfds[0], &buf, sizeof(PIPE_MSG));
        close(pfds[0]);
        if (tres != sizeof(PIPE_MSG))
            exit_err("parent:read");
        buf[sizeof(buf) - 1] = 0;
        printf("parent:read:%s\n", buf);
        exit(EXIT_SUCCESS);
    }
}
