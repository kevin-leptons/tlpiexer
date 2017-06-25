/*
SYNOPSIS

    Solution for exercise 63-07.

REQUIREMENT

    Suppose we have an epoll file descriptor that is monitoring multiple
    file descriptors, all of which are always ready. If we perform a series
    of epoll_wait() calls in which maxevents is much smaller than the number
    of ready file descriptors (e.g., maxevents is 1), without performing
    all possible I/O on the ready descriptors between calls, what
    descriptor(s) does epoll_wait() return in each call? Write a program to
    determine the answer (For the perposes of this experiment, it suffices
    to perform no I/O between the epoll_wait() system calls). Why might this
    behavior be useful?

SOLUTION

    Create n pipes and write few data to write-end of it.

    Build an epoll with n pipes above.

    Monitor epoll above with maxevents is one. Each time event occurs,
    display information about pipe and break monitor loop until
    number events has occurs equal n.

    As a result, epoll_wait() interate from first to last pipe instead of
    first pipe in list of pipes.

    It is useful because epoll_wait() interate foreach file descriptors
    in list instead of only first file descriptor.

USAGE

    $ ./dest/bin/63-07
    Create 8 pipes
    ... => pipe-write-end: 3
    ... => pipe-write-end: 5
    ... => pipe-write-end: 9
    ... => pipe-write-end: 11
    ... => pipe-write-end: 14
    ... => pipe-write-end: 16
    ... => pipe-write-end: 22
    ... => pipe-write-end: 24

AUTHORS

    Kevin Leptons <kevin.leptons@gmail.com>
*/

#include <stdlib.h>
#include <stdio.h>
#include <sys/epoll.h>
#include <unistd.h>

#include <tlpiexer/error.h>

#define EPOLL_FD_NUM 8

int main(int argc, char *argv[])
{
    int i;
    int j;
    int epoll_fd;
    struct epoll_event epoll_evt;
    struct epoll_event epoll_evts[EPOLL_FD_NUM];
    int pipe_fds[EPOLL_FD_NUM][2];
    ssize_t rd_num;

    // create and write data to pipes
    printf("Create %i pipes\n", EPOLL_FD_NUM);
    for (i = 0; i < EPOLL_FD_NUM; i++) {
        if (pipe(pipe_fds[i]) != 0)
            exit_err("pipe");
        if (write(pipe_fds[i][1], "x", 1) != 1)
            exit_err("write:pipe");
    }

    // build epoll data
    epoll_fd = epoll_create(EPOLL_FD_NUM);
    if (epoll_fd < 0)
        exit_err("epoll_create");
    for (i = 0; i < EPOLL_FD_NUM; i++) {
        epoll_evt.events = EPOLLIN;
        epoll_evt.data.fd = pipe_fds[i][0];
        epoll_ctl(epoll_fd, EPOLL_CTL_ADD, epoll_evt.data.fd, &epoll_evt);
    }

    // monitor on epoll
    for (j = 0; j < EPOLL_FD_NUM; j++) {
        rd_num = epoll_wait(epoll_fd, epoll_evts, 1, -1);
        if (rd_num < 0)
            exit_err("epoll_wait");
        for (i = 0; i < rd_num; i++) {
            if (!epoll_evts[i].events & EPOLLIN)
                continue;
            printf("... => pipe-write-end: %i\n", epoll_evts[i].data.fd);
        }
    }

    return EXIT_SUCCESS;
}
