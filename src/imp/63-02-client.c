/*
SYNOPSIS

    Part of solution for exercise 63-02. See 63-02.c for more information.

AUTHORS

    Kevin Leptons <kevin.leptons@gmail.com>
*/

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <tlpiexer/error.h>
#include <tlpiexer/59-04-isock.h>

void print_help(FILE *stream);
void send_udp_msg(const char *host, const char *port, const char *msg);
void send_tcp_msg(const char *host, const char *port, const char *msg);

int main(int argc, char *argv[])
{
    if (argc != 5) {
        print_help(stderr);
        return EXIT_FAILURE;
    }

    if (strcmp(argv[3], "udp") == 0) {
        send_udp_msg(argv[1], argv[2], argv[4]);
    }
    else if (strcmp(argv[3], "tcp") == 0) {
        send_tcp_msg(argv[1], argv[2], argv[4]);
    }
    else {
        print_help(stderr);
        return EXIT_FAILURE;
    }
        
    return EXIT_SUCCESS;
}

void send_udp_msg(const char *host, const char *port, const char *msg)
{
    int pfd;
    ssize_t wsize;
    ssize_t rsize;
    struct timeval tv;
    char buf[1024];
    struct sockaddr_in addr;

    pfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (pfd < 0)
        exit_err("send_udp_msg:socket");

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(atoi(port));
    if (inet_pton(AF_INET, host, &addr.sin_addr) != 0) 
        exit_err("send_udp_msg:inet_pton");

    tv.tv_sec = 0;
    tv.tv_usec = 100000;
    if (setsockopt(pfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) != 0)
        exit_err("send_udp_msg:setsockopt");

    wsize = sendto(pfd, msg, strlen(msg), 0,
                   (struct sockaddr*) &addr, sizeof(addr));
    if (wsize != strlen(msg))
        exit_err("send_tcp_msg:sendto");
    
    for (;;) {
        rsize = recv(pfd, buf, strlen(msg), 0);
        if (rsize < 0) {
            if (errno == EAGAIN)
                break;
            else
                exit_err("send_tcp_msg:recvfrom");
        }
        if (rsize == 0)
            break;

        buf[rsize] = 0;
        printf("%s", buf);
    }

    printf("\n");
}

void send_tcp_msg(const char *host, const char *port, const char *msg)
{
    int pfd;
    ssize_t wsize;
    ssize_t rsize;
    char buf[1024];

    pfd = isock_connect(host, port, SOCK_STREAM);
    if (pfd < 0)
        exit_err("send_tcp_msg:isock_connect");

    wsize = write(pfd, msg, strlen(msg));
    if (wsize != strlen(msg))
        exit_err("send_tcp_msg:write");
    
    for (;;) {
        rsize = read(pfd, buf, sizeof(buf));
        if (rsize < 0)
            exit_err("send_tcp_msg:read");
        if (rsize == 0)
            break;

        buf[rsize] = 0;
        printf("%s", buf);
    }

    printf("\n");
}

void print_help(FILE *stream)
{
    fprintf(stream, "Use: cmd <host> <port> <udp|tcp> <message>\n");
}
