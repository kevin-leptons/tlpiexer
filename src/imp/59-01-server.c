/*
SYNOPSIS

    Part of solution for exercise 59-01. See 59-01.c for more information.

AUTHORS

    Kevin Leptons <kevin.leptons@gmail.com>
*/

#include <stdlib.h>
#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>

#include <tlpiexer/error.h>
#include <tlpiexer/59-01.h>

#define BACKLOG 32

static int lfd = 0;
static int cfd = 0;
static size_t seq_num = 0;

static void term_handle(int sig);

int main(int argc, char *argv[])
{
    struct addrinfo hints;
    struct addrinfo *addr;
    struct addrinfo *caddr;
    struct addrinfo *a;
    int opt;
    ssize_t tres;
    size_t tsize;
    socklen_t caddr_len;
    char host[NI_MAXHOST];
    char service[NI_MAXSERV];
    char line[INT_LEN];
    struct lbuf lbuf;
    ssize_t req;

    if (signal(SIGPIPE, SIG_IGN) == SIG_ERR)        
        exit_err("server:signal:SIGPIPE");
    if (signal(SIGINT, term_handle) == SIG_ERR)
        exit_err("server:signal:SIGINT");
    if (signal(SIGTERM, term_handle) == SIG_ERR)
        exit_err("server:signal:SIGTERM");

    memset(&hints, 0, sizeof(hints));
    hints.ai_canonname = NULL;
    hints.ai_addr = NULL;
    hints.ai_next = NULL;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_family = AF_UNSPEC;
    hints.ai_flags = AI_PASSIVE | AI_NUMERICSERV;
    if (getaddrinfo(NULL, PORT_NUM, &hints, &addr) != 0) 
        exit_err("server:getaddrinfo");

    // chose andress to bind
    for (a = addr; a != NULL; a = a->ai_next) {
        lfd = socket(a->ai_family, a->ai_socktype, a->ai_protocol) ;
        if (lfd == FN_ER)
            continue;
        tres = setsockopt(lfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
        if (tres == FN_ER)
            exit_err("server:setsockopt");
        if (bind(lfd, a->ai_addr, a->ai_addrlen) == 0)
            break;
        close(lfd);
    }
    if (a == NULL)
        exit_err("server:coundn't bind socket to any address");

    // listen
    if (listen(lfd, BACKLOG) == FN_ER)
        exit_err("server:listen");
    freeaddrinfo(addr);

    // infinite loop to handle request from client
    for (;;) {
        // accept 
        caddr_len = sizeof(struct sockaddr_storage);
        cfd = accept(lfd, (struct sockaddr*) &caddr, &caddr_len);
        if (cfd == FN_ER) {
            msg_err("server:accept");
            continue;
        }

        // display peer information
        tres = getnameinfo((struct sockaddr*) &caddr, caddr_len, host,
                            sizeof(host), service, sizeof(service), 0);
        if (tres == FN_OK)
            printf("server:connect from %s:%s\n", host, service);
        else
            fprintf(stderr, "server:connect from UNKNOW\n");

        // read request
        if (init_lbuf(cfd, &lbuf)) {
            msg_err("server:init_lbuf");
            close(cfd);
            continue;
        }
        tres = read_lbuf(&lbuf, line, sizeof(line));
        if (tres <= 0) {
            msg_err("server:read");
            close(cfd);
            continue;
        }

        // process request
        req = atoi(line);
        if (req == 0) {
            msg_err("server:atoi");
            close(cfd);
            continue;
        }
        seq_num += req;

        // write response
        snprintf(line, sizeof(line), "%li\n", seq_num);
        tsize = strlen(line);
        tres = write(cfd, line, tsize);
        if (tres != tsize) {
            msg_err("server:write");
            close(cfd);
            continue;
        }

        if (close(cfd))
            msg_err("server:close");
        cfd = 0;
    }
    
    return EXIT_SUCCESS;
}

static void term_handle(int sig)
{
    if (lfd != 0)
        close(lfd);
    if (cfd != 0)
        close(cfd);
    signal(sig, SIG_DFL);
    kill(getpid(), sig);
}
