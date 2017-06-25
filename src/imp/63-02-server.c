/*
SYNOPSIS

    Part of solution for exercise 63-02. See 63-02.c for more information.

AUTHORS

    Kevin Leptons <kevin.leptons@gmail.com>
*/

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include <tlpiexer/error.h>

int udp_handle(int lfd);
int tcp_handle(int pfd);

int main(int argc, char *argv[])
{
    int lfd_udp;
    int lfd_tcp;
    int poll_fd;
    int pfd;
    int result;
    int i;
    struct sockaddr_in laddr;
    struct epoll_event poll_evt;
    struct epoll_event poll_revt[2];

    // verify arguments
    if (argc != 2) {
        fprintf(stderr, "Use: %s <port>\n", argv[0]);
        return EXIT_FAILURE;
    }

    // disable buffer for correct output
    setbuf(stdout, NULL);
    setbuf(stderr, NULL);

    // build network address
    memset(&laddr, 0, sizeof(laddr));
    laddr.sin_family = AF_INET;    
    laddr.sin_port = htons(atoi(argv[1]));
    laddr.sin_addr.s_addr = INADDR_ANY;
    
    // create socket
    lfd_udp = socket(AF_INET, SOCK_DGRAM, 0);
    if (lfd_udp < 0)
        exit_err("socket:udp");
    lfd_tcp = socket(AF_INET, SOCK_STREAM, 0);
    if (lfd_tcp < 0)
        exit_err("socket:tcp");

    // bind socket
    result = bind(lfd_udp, (struct sockaddr*) &laddr, sizeof(laddr));
    if (result != 0)
        exit_err("bind:udp");
    result = bind(lfd_tcp, (struct sockaddr*) &laddr, sizeof(laddr));
    if (result != 0)
        exit_err("bind:tcp");

    // listen on socket
    result = listen(lfd_tcp, 8);
    if (result != 0)
        exit_err("listen:tcp");

    // build epoll data
    poll_fd = epoll_create(2);
    if (poll_fd < 0)
        exit_err("epoll_create");
    poll_evt.events = EPOLLIN;
    poll_evt.data.fd = lfd_udp;
    if (epoll_ctl(poll_fd, EPOLL_CTL_ADD, lfd_udp, &poll_evt) != 0)
        exit_err("epoll_ctl:udp");
    poll_evt.data.fd = lfd_tcp;
    if (epoll_ctl(poll_fd, EPOLL_CTL_ADD, lfd_tcp, &poll_evt) != 0)
        exit_err("epoll_ctl:tcp");

    // accept connection from client
    // create new process to handle
    printf("Listen on port %s for UDP, TCP\n", argv[1]);
    for (;;) {
        result = epoll_wait(poll_fd, poll_revt, 2, -1);
        if (result < 0)
            exit_err("epoll_wait");
        for (i = 0; i < result; i++) {
            if (poll_revt[i].data.fd == lfd_udp) {
                udp_handle(lfd_udp);
            }
            else if (poll_revt[i].data.fd == lfd_tcp) {
                pfd = accept(lfd_tcp, NULL, NULL);
                if (pfd < 0) {
                    msg_err("accept");
                    continue;
                }
                tcp_handle(pfd);
            }
            else {
                continue;
            }
        }
    }
}

int udp_handle(int lfd)
{
    pid_t pid;
    struct sockaddr_in caddr;
    socklen_t socklen;
    ssize_t rsize;
    ssize_t wsize;
    char buf[1024];

    pid = fork();
    if (pid < 0) {
        return -1;
    }
    else if (pid == 0) {
        socklen = sizeof(caddr);
        memset(&caddr, 0, socklen);
        rsize = recvfrom(lfd, buf, sizeof(buf), 0,
                         (struct sockaddr*) &caddr, &socklen);
        if (rsize < 0)
            exit_err("udp_handle:recvfrom");

        buf[rsize] = 0;
        printf("... => UDP: %s\n", buf);

        wsize = sendto(lfd, buf, rsize, 0,
                       (struct sockaddr*) &caddr, socklen);
        if (wsize != rsize)
            exit_err("udp_handle:sendto");

        close(lfd);
        exit(0);
    } else {
        return 0;
    }
}

int tcp_handle(int pfd)
{
    ssize_t rsize;
    ssize_t wsize;
    char buf[1024];
    pid_t pid;

    pid = fork();
    if (pid < 0) {
        return -1;
    } else if (pid == 0) {
        rsize = read(pfd, buf, sizeof(buf));
        if (rsize < 0)
            exit_err("tcp_handle:read");

        buf[rsize] = 0;
        printf("... => TCP: %s\n", buf);

        wsize = write(pfd, buf, rsize);
        if (wsize != rsize)
            exit_err("tcp_handle:write");

        close(pfd);
        exit(0);
    } else {
        close(pfd);
        return 0;
    }
}
