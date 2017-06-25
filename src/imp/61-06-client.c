/*
SYNOPSIS

    Part of solution of exercise 61-06. See 61-06.c for more information.

AUTHRORS

    Kevin Leptons <kevin.leptons@gmail.com>
*/

#define _GNU_SOURCE

#include <stdlib.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <sys/wait.h>

#include <tlpiexer/error.h>
#include <tlpiexer/59-04-isock.h>
#include <tlpiexer/61-06.h>

#define DATA_NORMAL_SOCK "NORMAL NORMAL NORMAL"
#define DATA_URGENT_SOCK "URGENT URGENT URGENT"

static void send_dsock_entry(int pfd);

int main(int argc, char *argv[])
{
    int lfd = -1;
    int pfd = -1;
    int ofd = -1;
    struct sockaddr_in laddr;
    socklen_t laddr_len;
    struct handshake_msg hmsg;
    struct handshake_msg cf_hmsg;
    struct oob_msg oob_msg;
    ssize_t wsize;
    ssize_t rsize;
    pid_t pid;

    if (argc != 3) {
        fprintf(stderr, "Use: %s <addr> <port>\n", argv[0]);
        return EXIT_FAILURE;
    } 

    setbuf(stdout, NULL);
    setbuf(stderr, NULL);

    lfd = isock_listen("0", SOCK_STREAM, 1);
    if (lfd == -1)
        exit_err("isock_listen");
    memset(&laddr, 0, sizeof(laddr));
    laddr_len = sizeof(laddr);
    if (getsockname(lfd, (struct sockaddr*)&laddr, &laddr_len) != 0)
        exit_err("getsockname");
    printf("Urgent socket: %i\n", ntohs(laddr.sin_port));

    printf("Local => Server => Normal socket\n");
    pfd = isock_connect(argv[1], argv[2], SOCK_STREAM);
    if (pfd < 0) {
        msg_err("isock_connect");
        goto EXIT_ERROR;
    } 

    printf("Key => Normal socket\n");
    if (mk_key(hmsg.key) != 0) {
        msg_err("mk_key");
        goto EXIT_ERROR;
    }
    hmsg.port = ntohs(laddr.sin_port);
    wsize = write(pfd, &hmsg, sizeof(hmsg));
    if (wsize != sizeof(hmsg)) {
        msg_err("read");
        goto EXIT_ERROR;
    }

    printf("Urgent socket <= Local <= Server\n");
    ofd = accept(lfd, NULL, NULL);
    close(lfd);

    printf("Urgent socket <= Key\n");
    rsize = read(ofd, &cf_hmsg, sizeof(cf_hmsg)) ;
    if (rsize != sizeof(cf_hmsg)) {
        msg_err("read");
        goto EXIT_ERROR;
    }
    if (strncmp(cf_hmsg.key, hmsg.key, sizeof(hmsg.key)) != 0) {
        fprintf(
            stderr, "Key not match: correct=%s, confirm=%s\n", 
            hmsg.key, cf_hmsg.key
        );
        goto EXIT_ERROR;
    }

    printf("Data => Urgent socket: cmd=OK, data=N/A\n");
    oob_msg.code = OOB_OK;
    oob_msg.data[0] = 0;
    wsize = write(ofd, &oob_msg, sizeof(oob_msg));
    if (wsize != sizeof(oob_msg)) {
        msg_err("write");
        goto EXIT_ERROR;
    }

    pid = fork();
    if (pid == -1) {
        msg_err("fork");
        goto EXIT_ERROR;
    } else if (pid == 0) {
        send_dsock_entry(pfd);
        exit(1);
    }

    printf("Data => Urgent socket: cmd=DATA, data=%s\n", DATA_URGENT_SOCK);
    oob_msg.code = OOB_DATA;
    strcpy(oob_msg.data, DATA_URGENT_SOCK);
    wsize = write(ofd, &oob_msg, sizeof(oob_msg));
    if (wsize != sizeof(oob_msg)) {
        msg_err("oob_msg invalid");
        goto EXIT_ERROR;
    }

    sleep(3);
    printf("Data => Urgent socket: CLOSE\n");
    oob_msg.code = OOB_CLOSE;
    wsize = write(ofd, &oob_msg, sizeof(oob_msg));
    if (wsize != sizeof(oob_msg)) {
        msg_err("oob_msg invalid");
        goto EXIT_ERROR;
    }

    sleep(3);
    kill(pid, SIGKILL);
    wait(NULL);

    close(pfd);
    close(ofd);
    return EXIT_SUCCESS;

EXIT_ERROR:
    if (lfd > -1)
        close(lfd);
    if (pfd > -1)
        close(pfd);
    if (ofd > -1)
        close(ofd);
    return EXIT_FAILURE;
}
    
static void send_dsock_entry(int pfd)
{
    ssize_t wsize;
    struct pollfd fds[1];
    ssize_t n;

    fds[0].fd = pfd;
    fds[0].events = POLLRDHUP;

    for(;;) {
        n = poll(fds, 1, 1);
        if (n < 0)
            exit_err("send_dsock_entry:poll");
        if (n == 1 && (fds[0].revents & POLLRDHUP)) {
            fprintf(
                stderr, 
                "Data => Normal socket: ERROR: Server socket was closed\n"
            );
        } else {
            wsize = write(pfd, DATA_NORMAL_SOCK, sizeof(DATA_NORMAL_SOCK));
            if (wsize != sizeof(DATA_NORMAL_SOCK))
                exit_err("send_dsock_entry");
            printf("Data => Normal socket: %s\n", DATA_NORMAL_SOCK);
        }

        sleep(1);
    }
}
