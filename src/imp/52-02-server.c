/*
SYNOPSIS

    Part of solution for exercise 52-02. Goto 52-02.c for more information.

AUTHORS

    Kevin Leptons<kevin.leptons@gmail.com>
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <mqueue.h>
#include <signal.h>
#include <unistd.h>

#include <tlpiexer/error.h>
#include <tlpiexer/52-02.h>

#define FN_FAILURE -1
#define FN_SUCCESS 0

int handle_req(request_t *req);

void sig_term_handle(int sig);

int main(int argc, char *argv[])
{
    mqd_t mqd;
    int flags = O_CREAT | O_RDONLY;
    mode_t perms = S_IRUSR | S_IWUSR;
    struct mq_attr attr;
    request_t req;
    ssize_t rsize;
    struct sigaction sigact;

    // setup signal handler
    // to unlink message queue if process terminate
    sigact.sa_handler = sig_term_handle;
    sigact.sa_flags = SA_RESETHAND;
    sigemptyset(&sigact.sa_mask);
    if (sigaction(SIGINT, &sigact, NULL) == -1)
        exit_err("sigaction");
    if (sigaction(SIGTERM, &sigact, NULL) == -1)
        exit_err("sigaction");

    // create queue
    attr.mq_maxmsg = 8;
    attr.mq_flags = 0;
    attr.mq_msgsize = sizeof(request_t); 
    mqd = mq_open(MQ_PATH_52_2, flags, perms, &attr);
    if (mqd == -1)
        exit_err("server: mq_open");

    // infinite loop for receive message from client
    // terminate when received SIGTERM
    for (;;) {
        rsize = mq_receive(mqd, (char*)&req, sizeof(req), NULL);

        // system error
        // skip and continue read
        if (rsize < 0) {
            msg_err("rsize < 0");
            continue;
        }

        // data is incorrect
        // skip and continue read
        if (req.mq_name_size > (rsize - sizeof(req.mq_name_size))) {
            msg_err("req.rsize > (rsize - mq_name_size)");
            continue;
        }

        // send message back to client
        // if error occur, skip and continue read
        if (handle_req(&req) == FN_FAILURE) {
            msg_err("handle_req failed");
            continue;
        }
    }

    // remove and unlink queue
    if (mq_close(mqd) != 0)
        exit_err("mq_close");
    if (mq_unlink(MQ_PATH_52_2) != 0)
        exit_err("mq_unlink");

    return EXIT_SUCCESS;
}

int handle_req(request_t *req)
{
    static int seq_num = 0;
    mqd_t mqd;
    struct mq_attr attr;
    response_t res;

    // open client queue
    if ((mqd = mq_open(req->mq_name, O_WRONLY)) == -1)
        return FN_FAILURE;

    // retrieve limits of client queues
    // and verify size of response_t than msgsize
    if (mq_getattr(mqd, &attr) == -1)
        return FN_FAILURE;
    if (sizeof(response_t) > attr.mq_msgsize)
        return FN_FAILURE;

    // create message
    res.seq_num = seq_num; 

    // send message to client
    if (mq_send(mqd, (char*)&res, sizeof(res), 0) == -1)
        return FN_FAILURE;

    // increase sequence number
    seq_num++;

    // close client queue
    if (mq_close(mqd) == -1)
        return FN_FAILURE;

    return FN_SUCCESS;
}

void sig_term_handle(int sig)
{
    pid_t pid;
    
    // remove message queue if it exist
    if (mq_open(MQ_PATH_52_2, O_CREAT | O_EXCL) == -1)
        mq_unlink(MQ_PATH_52_2);

    // reset handler to default 
    // send signal again to continue handle
    pid = getpid();  
    kill(pid, sig);
}
