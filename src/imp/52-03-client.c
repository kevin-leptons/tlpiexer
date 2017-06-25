/*
SYNOPSIS

    Part of solution for exercise 52-03. Goto 52-03.c for more information.

AUTHORS

    Kevin Leptons <kevin.leptons@gmail.com>
*/

#include <stdlib.h>
#include <stdio.h>
#include <mqueue.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <unistd.h>

#include <tlpiexer/52-03.h>
#include <tlpiexer/random.h>

#define FN_FAILURE -1
#define FN_SUCCESS 0

void term_handle(int sig);

void exit_err(const char *msg);

static char client_name[PATH_MAX];

int main(int argc, char *argv[])
{
    mqd_t mqd_server, mqd_client;
    int cflags = O_CREAT | O_EXCL | O_RDONLY;
    mode_t cmodes = S_IRUSR | S_IWUSR;
    struct mq_attr cattr;
    response_t res;
    request_t req;
    ssize_t rsize;
    char buf[sizeof(res.data)];
    size_t rand_num;

    // verify argument
    if (argc != 2) {
        printf("Use: <cmd> <file>\n");
        return EXIT_FAILURE;
    }

    // setup signal handler
    if (signal(SIGINT, term_handle) == SIG_ERR)
        exit_err("signal");
    if (signal(SIGTERM, term_handle) == SIG_ERR)
        exit_err("signal");

    // open client-queue
    // 
    if (rrand(0, 999999, &rand_num) != 0)
        exit_err("rrand");
    sprintf(client_name, "%s-%06li", MQ_CLIENT_TMP, rand_num);
    cattr.mq_msgsize = sizeof(res);
    cattr.mq_maxmsg = 8;
    cattr.mq_flags = 0;
    if ((mqd_client = mq_open(client_name, cflags, cmodes, &cattr)) == -1)
        exit_err("mq_open");

    // open server-queue
    // create request and send to server-queue
    if ((mqd_server = mq_open(MQ_SERVER, O_WRONLY)) == -1)
        exit_err("mq_open");
    strcpy(req.mq_client, client_name);
    strcpy(req.file, argv[1]);
    if (mq_send(mqd_server, (char*)&req, sizeof(req), 0) == -1) {
        mq_close(mqd_client);
        mq_unlink(client_name);
        exit_err("client: mq_send");
    }

    // enter loop to receive parts of file
    // display to stdout
    // until end of file or error
    for (;;) {
        rsize = mq_receive(mqd_client, (char*)&res, sizeof(res), NULL);
        if (rsize != sizeof(res)) {
            mq_close(mqd_client);
            mq_unlink(client_name);
            exit_err("mq_receive");
        }

        if (res.state == ERROR)
            exit_err("state");
        if (res.state == END) {
            printf("\n");
            break;
        }

        // print part of file in text format
        snprintf(buf, res.size, "%s", res.data);
        printf("%s", buf);
    }

    // free resources
    mq_close(mqd_client);
    mq_unlink(client_name);
    mq_close(mqd_server);

    return EXIT_SUCCESS;
}

void term_handle(int sig)
{
    pid_t pid;

    // remove message queue if it exist
    if (mq_open(client_name, O_CREAT | O_EXCL) == -1)
        mq_unlink(client_name);

    // reset handler to default
    // send signal again to continue handle
    signal(sig, SIG_DFL);
    pid = getpid();
    kill(pid, sig);
}

void exit_err(const char *caller)
{
    // remove server-queue if it is exist
    if (mq_open(client_name, O_CREAT | O_EXCL) == -1)
        mq_unlink(client_name);

    perror(caller);
    perror(strerror(errno));
    exit(EXIT_FAILURE);
}
