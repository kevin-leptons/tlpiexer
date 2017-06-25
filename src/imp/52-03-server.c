/*
 * 
SYNOPSIS

    Part of solution for exercise 52-03. Goto 52-03.c for more information.

AUTHORS

    Kevin Leptons <kevin.leptons@gmail.com>
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <mqueue.h>
#include <signal.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>

#include <tlpiexer/52-03.h>

#define FN_FAILURE -1
#define FN_SUCCESS 0

void term_handle(int sig);

void exit_err(const char *msg);

int handle_req(request_t *req);

void warn(const char *msg);

int main(int argc, char *argv[])
{
    int flags = O_CREAT | O_EXCL | O_RDONLY;
    mode_t mode = S_IRUSR | S_IWUSR;
    struct mq_attr attr;
    mqd_t mqd;
    ssize_t rsize;
    request_t req;

    // setup signal handler for terminate
    if (signal(SIGINT, term_handle) == SIG_ERR)
        exit_err("signal");
    if (signal(SIGTERM, term_handle) == SIG_ERR)
        exit_err("signal");

    // open server-queue
    attr.mq_msgsize = sizeof(request_t);
    attr.mq_maxmsg = 8;
    attr.mq_flags = 0;
    if ((mqd = mq_open(MQ_SERVER, flags, mode, &attr)) == -1)
        exit_err("mq_open");

    // enter infinite-loop to handle requests
    for (;;) {
        // receive and verify message
        // skip error and continue receive next request
        rsize = mq_receive(mqd, (char*)&req, sizeof(req), NULL);
        if (rsize == -1){
            warn("rsize = -1");
            continue;
        }
        if (rsize != sizeof(req)) {
            warn("rsize != sizeof(req)");
            continue;
        }

        // call handler
        if (handle_req(&req) == FN_FAILURE)
            warn("handle_req()");
    }

    return EXIT_SUCCESS;
}

void exit_err(const char *caller)
{
    // remove server-queue if it is exist
    if (mq_open(MQ_SERVER, O_CREAT | O_EXCL) == -1)
        mq_unlink(MQ_SERVER);

    perror(caller);
    perror(strerror(errno));
    exit(EXIT_FAILURE);
}

int handle_req(request_t *req)
{
    mqd_t mqd;
    struct mq_attr attr;
    response_t res;
    int fd;
    ssize_t rsize;

    // open client queue
    if ((mqd = mq_open(req->mq_client, O_WRONLY)) == -1) {
        warn("server: mq_open -> client");
        return FN_FAILURE;
    }

    // retrieve limits of client queues
    // verify size of response_t than msgsize
    if (mq_getattr(mqd, &attr) == -1) {
        mq_close(mqd);
        warn("server: mq_getattr");
        return FN_FAILURE;
    }
    if (sizeof(response_t) != attr.mq_msgsize) {
        mq_close(mqd);
        warn("client msgsize != sizeof(response_t)");
        return FN_FAILURE;
    }

    // open file
    if ((fd = open(req->file, O_RDONLY)) == -1) {
        mq_close(mqd);
        warn("server: open -> file");
        return FN_FAILURE;
    }

    // loop to read, send data block of file to client
    for (;;) {
        // read part of file
        errno = 0;
        rsize = read(fd, (void*)res.data, sizeof(res.data));
        if (rsize == 0 && errno != 0) {
            res.state = ERROR;
            res.size = 0;
            if (mq_send(mqd, (char*)&res, sizeof(response_t), 0) == -1)
                warn("mq_send");
            break;
        } else if (rsize == 0) {
            res.state = END;
        } else {
            res.state = PART;
        }

        // send part of file to client
        res.size = (int)rsize;
        if (mq_send(mqd, (char*)&res, sizeof(response_t), 0) == -1) {
            warn("mq_send");
            break;
        }
        if (rsize == 0)
            break;
    }

    // free resources
    mq_close(mqd);
    close(fd);

    return FN_SUCCESS;
}

void term_handle(int sig)
{
    pid_t pid;

    // remove message queue if it exist
    if (mq_open(MQ_SERVER, O_CREAT | O_EXCL) == -1)
        mq_unlink(MQ_SERVER);

    // reset handler to default
    // send signal again to continue handle
    signal(sig, SIG_DFL);
    pid = getpid();
    kill(pid, sig);
}

void warn(const char *msg)
{
    printf("warn: %s\n", msg);
    printf("\t error: %s\n", strerror(errno));
}
