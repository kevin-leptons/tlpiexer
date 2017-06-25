/*
SYNOPSIS

    Part of solution of exercise 61-06. See 61-06.c for more information.

AUTHRORS

    Kevin Leptons <kevin.leptons@gmail.com>
*/

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

#include <tlpiexer/error.h>
#include <tlpiexer/61-06.h>
#include <tlpiexer/59-04-isock.h>

static void peer_proc_entry(int pfd, struct sockaddr_in *addr);
static int handshake_peer(int pfd, struct sockaddr_in *sock);

int main(int argc, char *argv[])
{
    int lfd;
    int pfd;
    struct sockaddr_in caddr;
    socklen_t caddr_len;
    pid_t pid;

    if (argc != 2) {
        printf("Use: %s <port>\n", argv[0]);
        return 1;
    }

    setbuf(stdout, NULL);
    setbuf(stderr, NULL);
    signal(SIGCHLD, SIG_IGN);

    lfd = isock_listen(argv[1], SOCK_STREAM, 8);
    caddr_len = sizeof(caddr);

    printf("Ready\n");
    for (;;) {
        memset(&caddr, 0, sizeof(caddr));
        pfd = accept(lfd, (struct sockaddr*)&caddr, &caddr_len);
        if (pfd < 0) {
            msg_err("accept");
            goto EXIT_ERROR;
        }
        printf("Normal socket <= Server <= Client: %i\n", caddr.sin_port);
        pid = fork();
        if (pid == -1) {
            exit_err("fork");
        } else if (pid == 0) {
            peer_proc_entry(pfd, &caddr);
            exit(1);
        } else {
            close(pfd);
        }
    }

    close(lfd);
    return EXIT_SUCCESS;

EXIT_ERROR:
    if (lfd > -1)
        close(lfd);
    exit(EXIT_FAILURE);
}

static void peer_proc_entry(int pfd, struct sockaddr_in *addr)
{
    int ofd = -1;
    int r;
    struct oob_sock obs;
    char buf[1024];
    struct oob_msg oob_msg;
    ssize_t rsize;

    ofd = handshake_peer(pfd, addr);
    if (ofd < 0) {
        msg_err("handshake_peer");
        goto EXIT_ERROR;
    }

    mk_oob_sock(pfd, ofd, &obs);

    for (;;) {
        r = is_oob(&obs);
        if (r == -1) {
            msg_err("is_oob");
            goto EXIT_ERROR;
        }
        if (r == 0)
            continue;
        if (r == 2) {
            if(read(pfd, buf, sizeof(buf)) == -1)
                goto EXIT_ERROR;
            fprintf(stdout, "Normal socket <= Client: %s\n", buf);
        }
        else if (r == 1) {
            rsize = read(ofd, &oob_msg, sizeof(oob_msg));
            if (rsize != sizeof(oob_msg)) {
                msg_err("ofd: oob_msg size is invalid\n");
                goto EXIT_ERROR;
            }
            if (oob_msg.code == OOB_CLOSE) {
                printf("Urgent socket <= Client: cmd=CLOSE\n");
                oob_msg.code = OOB_OK;
                write(ofd, &oob_msg, sizeof(oob_msg));
                break;
            } else if (oob_msg.code == OOB_DATA) {
                printf(
                    "Urgent socket <= Client: cmd=DATA, data=%s\n", 
                    oob_msg.data
                );
            } else {
                printf("Urgent socket <= Client: cmd=UNKNOW\n");
            }
        }
    }
    
    close(pfd);
    close(ofd);
    exit(0);

EXIT_ERROR:
    if (pfd >= 0)
        close(pfd);
    if (ofd >= 0)
        close(ofd);
    exit(1);
}

static int handshake_peer(int pfd, struct sockaddr_in *addr)
{
    struct handshake_msg hmsg;
    struct oob_msg oob_msg;
    ssize_t n;
    int ofd = -1;
    char paddr_str[INET_ADDRSTRLEN];
    const char *p;
    char pport_str[20];

    printf("Key <= Normal socket\n");
    memset(&hmsg, 0, sizeof(hmsg));
    n = read(pfd, &hmsg, sizeof(hmsg));
    if (n != sizeof(hmsg)) {
        msg_err("read");
        goto EXIT_ERROR;
    }

    memset(paddr_str, 0, sizeof(paddr_str));
    p = inet_ntop(
        AF_INET, &(addr->sin_addr), paddr_str, sizeof(paddr_str)
    );
    if (p == NULL) {
        msg_err("inet_ntop");
        goto EXIT_ERROR;
    }
    snprintf(pport_str, sizeof(pport_str), "%i", hmsg.port);
    printf("Server => %s:%s => Urgent socket\n", paddr_str, pport_str);
    ofd = isock_connect(paddr_str, pport_str, SOCK_STREAM);
    if (ofd == -1) {
        msg_err("isock_connect");
        goto EXIT_ERROR;
    }

    printf("Key => Urgent socket\n");
    n = write(ofd, &hmsg, sizeof(hmsg));
    if (n != sizeof(hmsg)) {
        msg_err("write");
        goto EXIT_ERROR;
    }

    printf("Urgent socket <= Key\n");
    memset(&oob_msg, 0, sizeof(oob_msg));
    n = read(ofd, &oob_msg, sizeof(oob_msg));
    if (n != sizeof(oob_msg)) {
        msg_err("read");
        goto EXIT_ERROR;
    }
    if (oob_msg.code != OOB_OK) {
        msg_err("Client did not accept\n");
        goto EXIT_ERROR;
    }

    return ofd;

EXIT_ERROR:
    if (ofd >= 0)
        close(ofd);
    return -1;
}
