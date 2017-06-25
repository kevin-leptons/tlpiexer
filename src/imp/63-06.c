/*
SYNOPSIS

    Solution for exercise 63-06.

REQUIREMENT

    Write a program that uses epoll_create() to create an epoll instance and
    then immediately waits on the returned file descriptor using epoll_wait().
    When, as in this case, epoll_wait() is given an epoll file descriptor
    with an empty interest list, what happens? Why might this be useful?

SOLUTION

    Write program to verify "What is happen?".

    epoll_wait() will be block. It is useful in multi-threading program,
    when one thread call epoll_wai() and other threads call epoll_ctl()
    to add file descriptor to monitor.

USAGE

    ./dest/bin/63-6

AUTHORS

    Kevin Leptons <kevin.leptons@gmail.com>
*/

#include <stdlib.h>
#include <stdio.h>
#include <sys/epoll.h>
#include <string.h>

#include <tlpiexer/error.h>

#define EPOLL_EVT_NUM 1
#define EPOLL_TIMEOUT 3000

int main(int argc, char *argv[])
{
    int epoll_fd;
    struct epoll_event epoll_evts[EPOLL_EVT_NUM];
    ssize_t result;

    epoll_fd = epoll_create(1);
    memset(epoll_evts, 0, sizeof(epoll_evts));

    printf("Start waiting on empty epoll_fd\n");
    result = epoll_wait(epoll_fd, epoll_evts, EPOLL_EVT_NUM, EPOLL_TIMEOUT);
    if (result == 0)
        printf("Finish waiting: timeout\n");
    else if (result > 0)
        printf("Finish waiting: %li event\n", result);
    else
        exit_err("epoll_wait");

    return EXIT_SUCCESS;
}
