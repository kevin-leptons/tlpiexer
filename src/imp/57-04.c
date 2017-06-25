/*
SYNOPSIS

    Solution for excercise 57-04.

REQUIREMENT

    Suppose that we create two UNIX domain datagram sockets bound to the
    paths /somepath/a and /somepath/b, and that we connect the socket
    /somepath/a to /somepath/b. What happens if we create a third datagram
    socket and try to send (sendto()) a datagram via that socket to
    /somepath/a? Write a program to determine the answer. If you have access
    to other UNIX systems, test the program on those systems to see if the
    answer differs.

SOLUTION

    All of sockets below are <AF_UNIX, SOCK_DGRAM>

    Create child process called A. In A, create and bind socket called AS.

    Create child process called B. In B, create and bind socket called BS.
    Then connect BS to AS.

    Parent process called C. In C, create and bind socket called CS then
    send a message to BS.

    See "What is happen?".

USAGE

    # in case BS isn't connect to AS
    $ ./dest/bin/57-04
    c:wait b:1 secs
    b:receive:hello there

    # in case BS connect to AS
    $ ./dest/bin/57-04 -c
    c:wait b:1 secs
    error:c:sendto: Operation not permitted

AUTHORS

    Kevin Leptons <kevin.leptons@gmail.com>
*/

#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <stdbool.h>
#include <getopt.h>

#include <tlpiexer/error.h>

#define SOCK_A_PATH "/tlpiexer-57-04-sock-a"
#define SOCK_B_PATH "/tlpiexer-57-04-sock-b"
#define SOCK_C_PATH "/tlpiexer-57-04-sock-c"
#define BUF_SIZE 1024
#define PROC_C_WAIT_B_SECS 1

void proc_a(void);
void proc_b(bool iscon);
void proc_c(pid_t apid, pid_t bpid);

int main(int argc, char *argv[])
{
    pid_t apid;
    pid_t bpid;
    bool connect = false;
    int opt;

    // parse arguments
    for (;;) {
        opt = getopt(argc, argv, "c");
        if (opt == -1)
            break;
        else if (opt == 'c')
            connect = true;
        else
            exit_usage("Use: cmd [-c]");
    }

    // disable stdout buffer to get correct output
    setbuf(stdout, NULL);

    // create proc a
    apid = fork();
    if (apid == FN_ER)
        exit_err("parent:fork:a");
    else if (apid == 0)
        proc_a(); // it terminate after done

    // create proc b
    bpid = fork();
    if (bpid == FN_ER)
        exit_err("parent:fork:b");
    else if (bpid == 0)
        proc_b(connect); // it terminate process after done

    // parent process is proc c
    proc_c(apid, bpid);
}

void proc_a(void)
{
    int sock_fd;
    struct sockaddr_un sock_addr;
    ssize_t res;
    char buf[BUF_SIZE];

    memset(&sock_addr, 0, sizeof(sock_addr));
    sock_addr.sun_family = AF_UNIX;
    sock_addr.sun_path[0] = 0;
    strncpy(sock_addr.sun_path + 1, SOCK_A_PATH,
            sizeof(sock_addr.sun_path) - 2);

    sock_fd = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (sock_fd == FN_ER)
        exit_err("a:socket");

    res = bind(sock_fd, (struct sockaddr*) &sock_addr, sizeof(sock_addr));
    if (res == FN_ER)
        exit_err("a:bind");

    // wait for sock_b connect
    for (;;) {
        res = recvfrom(sock_fd, buf, sizeof(buf), 0, NULL, NULL);
        if (res == FN_ER)
            continue;
    }

    exit(EXIT_SUCCESS);
}

void proc_b(bool iscon)
{
    int bsock_fd;
    struct sockaddr_un asock_addr;
    struct sockaddr_un bsock_addr;
    ssize_t res;
    char buf[BUF_SIZE];

    memset(&asock_addr, 0, sizeof(asock_addr));
    asock_addr.sun_family = AF_UNIX;
    asock_addr.sun_path[0] = 0;
    strncpy(asock_addr.sun_path + 1, SOCK_A_PATH,
            sizeof(asock_addr.sun_path) - 2);

    memset(&bsock_addr, 0, sizeof(bsock_addr));
    bsock_addr.sun_family = AF_UNIX;
    bsock_addr.sun_path[0] = 0;
    strncpy(bsock_addr.sun_path + 1, SOCK_B_PATH,
            sizeof(bsock_addr.sun_path) - 2);

    bsock_fd = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (bsock_fd == FN_ER)
        exit_err("b:socket");

    res = bind(bsock_fd, (struct sockaddr*) &bsock_addr, sizeof(bsock_addr));
    if (res == FN_ER)
        exit_err("b:bind");

    if (iscon) {
        res = connect(bsock_fd, (struct sockaddr*) &asock_addr,
                      sizeof(asock_addr));
        if (res == FN_ER)
            exit_err("b:connect");
    }

    // wait message from sock_a to don't terminate process b
    for (;;) {
        res = recvfrom(bsock_fd, buf, sizeof(buf), 0, NULL, NULL);
        if (res == -1)
            continue;
        printf("b:receive: %s\n", buf);
    }

    exit(EXIT_SUCCESS);
}

void proc_c(pid_t apid, pid_t bpid)
{
    int csock_fd;
    struct sockaddr_un bsock_addr;
    struct sockaddr_un csock_addr;
    ssize_t res;
    char buf[BUF_SIZE];

    memset(&bsock_addr, 0, sizeof(bsock_addr));
    bsock_addr.sun_family = AF_UNIX;
    bsock_addr.sun_path[0] = 0;
    strncpy(bsock_addr.sun_path + 1, SOCK_B_PATH,
            sizeof(bsock_addr.sun_path) - 2);

    memset(&csock_addr, 0, sizeof(csock_addr));
    csock_addr.sun_family = AF_UNIX;
    csock_addr.sun_path[0] = 0;
    strncpy(csock_addr.sun_path + 1, SOCK_C_PATH,
            sizeof(csock_addr.sun_path) - 2);

    csock_fd = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (csock_fd == FN_ER)
        goto ERROR;

    res = bind(csock_fd, (struct sockaddr*) &csock_addr, sizeof(csock_addr));
    if (res == FN_ER)
        goto ERROR;

    printf("c:wait b:%u secs\n", PROC_C_WAIT_B_SECS);
    sleep(PROC_C_WAIT_B_SECS);
    strcpy(buf, "hello there");
    res = sendto(csock_fd, buf, sizeof(buf), 0,
                 (struct sockaddr*) &bsock_addr, sizeof(bsock_addr));
    if (res == FN_ER) {
        fprintf(stderr, "error:c:sendto:%s\n", strerror(errno));
        goto ERROR;
    }

    // wait for process b display message
    sleep(PROC_C_WAIT_B_SECS);

    // kill all of children and wait it to terminate
    if (kill(apid, SIGKILL) == FN_ER)
        exit_err("c:kill:a");
    if (kill(bpid, SIGKILL) == FN_ER)
        exit_err("c:kill:b");
    wait(NULL);

    exit(EXIT_SUCCESS);

ERROR:
    if (kill(apid, SIGKILL) == FN_ER)
        exit_err("c:kill:a");
    if (kill(bpid, SIGKILL) == FN_ER)
        exit_err("c:kill:b");
    wait(NULL);
    exit(EXIT_FAILURE);
}
