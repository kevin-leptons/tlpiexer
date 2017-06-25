/*
SYNOPSIS

    Solution for excercise 59-05.

REQUIREMENT

    Suppose that we create two Internet domain datagram sockets, bound to
    specific addresses, and connect the first socket to the second. What
    happens if we create a third datagram socket and try to send (sendto())
    a datagram via that socket to the first socket? Write a program to
    determine the answer.

SOLUTION

    Call sendto() with third socket to first socket doesn't cause error,
    because it is SOCK_DGRAM, and no error responses. First socket perform
    connect() to second socket so it can't receive any data from other
    than second socket.

    Create three process coresspond with three socket, each process performs
    operations such as requirements. In both case, no error responses.
    But in case no perform connect() first socket to second socket, first
    socket can receive data from third socket.

    Use inet socket library in "include/tlpiexer/59-04-isock.h" to reduce
    code.

USAGE

    # doesn't connect first socket to second socket
    # no error responses
    # but first socket receive data from third socket
    $ ./dest/bin/59-05
    proc_first:recvfrom:proc_third:hello there

    # connect first socket to second socket
    # no error responses
    # first socket doesn't receive data from thrid socket
    $ ./dest/bin/59-05 -c

AUTHORS

    Kevin Leptons <kevin.leptons@gmail.com>
*/

#include <stdlib.h>
#include <stdio.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <errno.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdbool.h>

#include <tlpiexer/error.h>
#include <tlpiexer/59-04-isock.h>

#define PFIRST_WAIT_PSECOND 1
#define PTHIRD_WAIT_PFIRST PFIRST_WAIT_PSECOND + 1
#define PTHIRD_WAIT_CHILD 1
#define PORT_FIRST "50001"
#define PORT_SECOND "50002"
#define PORT_THIRD "5003"
#define BUF_SIZE 1024
#define BACKLOG 8

static int sfd_first = 0;
static int sfd_second = 0;
static int sfd_third = 0;
static bool is_connect = false;

static pid_t pid_first = 0;
static pid_t pid_second = 0;

static void proc_first(void);
static void proc_second(void);
static void proc_third(void);

static void thandle_first(int sig);
static void thandle_second(int sig);
static void thandle_third(int sig);

static void free_res(void);

int main(int argc, char *argv[])
{
    int opt;

    // parse argument
    if (argc > 2) {
        fprintf(stderr, "Use: %s [-c]", argv[0]);
        return EXIT_FAILURE;
    }
    for (;;)  {
        opt = getopt(argc, argv, "c");
        if (opt < 0)
            break;
        if (opt == 'c')
            is_connect = true;
        else {
            fprintf(stderr, "Use: %s [-c]", argv[0]);
            return EXIT_FAILURE;
        }
    }

    // disable i/o buffer to get correct output
    setbuf(stdout, NULL);
    setbuf(stderr, NULL);

    // create second process
    pid_second = fork();
    if (pid_second == FN_ER)
        exit_err("fork:second");
    else if (pid_second == 0)
        proc_second();

    // create first process
    pid_first = fork();
    if (pid_first == FN_ER)
        exit_err("fork:first");
    else if (pid_first == 0)
        proc_first();

    // third process
    proc_third();
}

static void proc_first(void)
{
    struct sockaddr_in addr_second;
    struct sockaddr_in paddr;
    socklen_t paddr_len;
    int tres;
    ssize_t rsize;
    char buf[BUF_SIZE];

    if (signal(SIGTERM, thandle_first) == SIG_ERR)
        exit_err("proc_first:signal");

    // create socket
    sfd_first = socket(AF_INET, SOCK_DGRAM, 0);
    if (sfd_first == FN_ER)
        exit_err("proc_first:socket");

    // bind socket
    if (isock_bind(sfd_first, PORT_FIRST, SOCK_DGRAM) == FN_ER)
        exit_err("proc_first:isock_bind");

    // wait second process initialization
    sleep(PFIRST_WAIT_PSECOND);

    // connect to second socket
    if (is_connect) {
        memset(&addr_second, 0, sizeof(addr_second));
        if (inet_pton(AF_INET, "127.0.0.1", &addr_second.sin_addr) != 1)
            exit_err("proc_first:inet_pton");
        addr_second.sin_family = AF_INET;
        addr_second.sin_port = htons(atoi(PORT_SECOND));
        tres = connect(sfd_first, (struct sockaddr*) &addr_second,
                       sizeof(addr_second));
        if (tres == FN_ER)
            exit_err("proc_first:connect");
    }

    // receive data
    paddr_len = sizeof(paddr);
    for (;;) {
        rsize = recvfrom(sfd_first, buf, sizeof(buf), 0,
                         (struct sockaddr*) &paddr, &paddr_len);
        if (rsize == FN_ER)
            continue;
        printf("proc_first:recvfrom:%s\n", buf);
    }
}

static void proc_second(void)
{
    ssize_t tres;

    if (signal(SIGTERM, thandle_second) == SIG_ERR)
        exit_err("proc_second:signal");

    // create socket
    sfd_second = socket(AF_INET, SOCK_DGRAM, 0);
    if (sfd_second == FN_ER)
        exit_err("proc_second:socket");

    // bind socket
    tres = isock_bind(sfd_second, PORT_SECOND, SOCK_DGRAM);
    if (tres == FN_ER)
        exit_err("proc_second:isock_listen");
    if (tres > 0) {
        fprintf(stderr, "proc_second:isock_bind:%s\n", gai_strerror(tres));
        exit(EXIT_FAILURE);
    }

    // wait for first process connect to
    // wait for third process kill it
    getchar();
}

static void proc_third(void)
{
    int tres;
    char buf[BUF_SIZE];
    struct sockaddr_in addr_first;

    if (signal(SIGTERM, thandle_third) == SIG_ERR)
        goto ERROR;
    if (signal(SIGINT, thandle_third) == SIG_ERR)
        goto ERROR;

    // create socket
    sfd_third = socket(AF_INET, SOCK_DGRAM, 0);
    if (sfd_third == FN_ER)
        goto ERROR;

    // bind socket
    tres = isock_bind(sfd_third, PORT_THIRD, SOCK_DGRAM);
    if (tres == FN_ER)
        goto ERROR;
    if (tres > 0) {
        fprintf(stderr, "proc_third:isock_bind:%s\n", gai_strerror(tres));
        goto ERROR;
    }

    // wait for first process initialization
    sleep(PTHIRD_WAIT_PFIRST);

    // try perform sendto()
    memset(&addr_first, 0, sizeof(addr_first));
    if (inet_pton(AF_INET, "127.0.0.1", &addr_first.sin_addr) != 1)
        goto ERROR;
    addr_first.sin_family = AF_INET;
    addr_first.sin_port = htons(atoi(PORT_FIRST));
    strcpy(buf, "proc_third:hello there");
    tres = sendto(sfd_third, buf, sizeof(buf), 0,
                  (struct sockaddr*) &addr_first, sizeof(addr_first));
    if (tres == FN_ER)
        goto ERROR;

    // wait first process write output
    sleep(PTHIRD_WAIT_CHILD);

    // success
    free_res();
    exit(EXIT_SUCCESS);

ERROR:
    free_res();
    if (errno != FN_OK)
        fprintf(stderr, "proc_third:%s\n", strerror(errno));
    exit(EXIT_FAILURE);
}

static void thandle_first(int sig)
{
    if (sfd_first > 0)
        close(sfd_first);
    signal(sig, SIG_DFL);
    kill(getpid(), sig);
}

static void thandle_second(int sig)
{
    if (sfd_second > 0)
        close(sfd_second);
    signal(sig, SIG_DFL);
    kill(getpid(), sig);
}

static void thandle_third(int sig)
{
    free_res();
    signal(sig, SIG_DFL);
    kill(getpid(), sig);
}

static void free_res(void)
{
    // terminate children
    if (pid_first > 0)
        kill(pid_first, SIGTERM);
    if (pid_second > 0)
        kill(pid_second, SIGTERM);
    waitpid(pid_first, NULL, WNOHANG);
    waitpid(pid_second, NULL, WNOHANG);

    // close socket
    if (sfd_third > 0)
        close(sfd_third);
}
