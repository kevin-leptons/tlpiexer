/*
SYNOPSIS

    Part of solution for exercise 52-02. Goto 52-02.c for more information.

AUTHORS

    Kevin Leptons <kevin.leptons@gmail.com>
*/

#include <stdlib.h>
#include <stdio.h>
#include <mqueue.h>
#include <errno.h>
#include <string.h>

#include <tlpiexer/error.h>
#include <tlpiexer/52-02.h>
#include <tlpiexer/random.h>

int main(int argc, char *argv[])
{
    mqd_t mqd_client, mqd_server;
    char mq_client[sizeof(MQ_PATH_52_2_CLIENT_TMP)];
    int mq_client_flags = O_CREAT | O_EXCL | O_RDONLY;
    int mq_client_mode = S_IRUSR | S_IWUSR;
    struct mq_attr attr_server, attr_client;
    request_t req;
    response_t res;
    ssize_t rsize;
    size_t rand_num;

    // open client-queue
    attr_client.mq_msgsize = sizeof(response_t);
    attr_client.mq_maxmsg = 1;
    attr_client.mq_flags = 0;
    if (rrand(0, 999999, &rand_num) != 0)
        exit_err("rrand");
    sprintf(mq_client, "%s-%06li", MQ_PATH_52_2_CLIENT_TMP, rand_num);
    mqd_client = mq_open(mq_client, mq_client_flags, mq_client_mode, 
            &attr_client);
    if (mqd_client == -1)
        exit_err("mq_open client");

    // open server-queue 
    if ((mqd_server = mq_open(MQ_PATH_52_2, O_WRONLY)) == -1)
        exit_err("mq_open server");

    // get limits of server-queue
    // verify size of message of server-queue than size of request
    if (mq_getattr(mqd_server, &attr_server) == -1)
        exit_err("mq_getattr");
    if (attr_server.mq_msgsize < sizeof(request_t))
        exit_err("sizeof(server-msg) < sizeof(request_t)\n");

    // send request to server
    strcpy(req.mq_name, mq_client);
    req.mq_name_size = strlen(mq_client);
    if (mq_send(mqd_server, (char*)&req, sizeof(req), 0) == -1)
        exit_err("mq_send");

    // wait and read response from server
    // verify response
    rsize = mq_receive(mqd_client, (char*)&res, sizeof(res), NULL);
    if (rsize == -1)
        exit_err("client: mq_receive");
    if (rsize != sizeof(res))
        exit_err("rsize != sizeof(response_t)");

    // free queues
    mq_close(mqd_client);
    mq_unlink(mq_client);
    mq_close(mqd_server);

    // display sequence number
    printf("response: %i\n", res.seq_num);
     
    return EXIT_SUCCESS;
}
