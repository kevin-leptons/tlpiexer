/*
SYNOPSIS

    Solution for exercise 52-04.

REQUIREMENT

    Write simple chat program (similar to talk(1), but without the curses
    interface) using POSIX messages queues.

SOLUTION

    Setup handler for SIGINT, SIGTERM to release queues.

    Parse option to chose server flow or client flow.

    SERVER FLOW

        Program create server-queue with name "tlpiexer-52-04-<user>". Where
        user is username of current user.

        Receive request to start chating from server-queue. If type of message
        is "START":

            Create new thread to read message from listen queue and denie
            all of "START" message. Then display message to stdout.

            Open client-queue by name "tlpiexer-52-04-<user>" with user from
            request. Read line from command line, send data to client-queue.

    CLIENT FLOW

        Open client-queue with name "tlpiexer-52-04-<user>". Where use is
        username of current user.

        Open server-queue with name "tlpiexer-52-04<user>". Where user is
        username of user who want to talk with.

        Send "START" message to server-queue.

        Read message form client-queue, if server send "ACCEPTED" message:

            Create new thread to receive and display message from server
            Enter loop to get line from command line and send to server

        If server didn't accept, exit with error

USAGE

    ROOT USER

    $ whoami
    root

    # start listen and wait for kevin user to talk
    # then continue in kevin user below
    $ ./dest/bin/52-04 -l

    # message from kevin user
    kevin: hello there

    # type something to kevin
    hi

    KEVIN USER

    $ whoami
    kevin

    # start talk with root user
    $ ./dest/bin/52-04 -c root
    invoke 0
    hello there

    # message from root user
    root: hi

AUTHORS

    Kevin Leptons <kevin.leptons@gmail.com>
*/

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <mqueue.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include <getopt.h>
#include <stdbool.h>
#include <pthread.h>
#include <sys/stat.h>

#define FN_FAILURE -1
#define FN_SUCCESS 0
#define MQ_USER_TMP "/tlpiexer-52-04-"
#define PATH_MAX 255
#define USER_SIZE 16
#define MSG_SIZE 1024

typedef enum {
    START = 0,
    ACCEPTED = 1,
    MSG = 2,
    END = 3,
    ERROR = 4
} reqtype_t;

typedef struct {
    reqtype_t type;
    char user[USER_SIZE];
    char data[MSG_SIZE];
} msg_t;

static char mq_listen_name[PATH_MAX], mq_peer_name[PATH_MAX];
static mqd_t mqd_listen, mqd_peer;
static char *user = NULL, peer[USER_SIZE];
static pthread_t receive_thread;

void *receive_msg_entry(void *argv);

void exit_err(const char *msg);

void warn(const char *msg);

int mq_user_name(char *mq_name, char *user);

void listen_user();

void invoke_peer();

void term_handle(int sig);

void exit_usage();

void start_message();

int main(int argc, char *argv[])
{
    int opt;
    bool is_listen = false, is_invoke = false;
    int flags = O_CREAT | O_EXCL | O_RDONLY;
    mode_t mode = S_IRUSR | S_IWUSR |  S_IROTH | S_IWOTH;
    struct mq_attr attr;
    mode_t omask;

    // setup signal handler to free resources
    if (signal(SIGINT, term_handle) == SIG_ERR)
        exit_err("signal");
    if (signal(SIGTERM, term_handle) == SIG_ERR)
        exit_err("signal");

    // init
    if ((user = getenv("USER")) == NULL)
        exit_err("getenv");
    mq_user_name(mq_listen_name, user);
    mq_peer_name[0] = 0;

    // parse options
    while ((opt = getopt(argc, argv, "lc:")) >= 0) {
        switch(opt) {
            case 'l':
                is_listen = true;
                break;
            case 'c':
                is_invoke = true;
                strcpy(peer, optarg);
                break;
            default:
                exit_usage();
        }
    }
    if ((is_listen && is_invoke) || !(is_listen || is_invoke))
        exit_usage();

    // change umask to help other process have write permision with queue
    if ((omask = umask(0)) == -1)
        exit_err("umask");

    // open listen-queue
    attr.mq_maxmsg = 8;
    attr.mq_flags = 0;
    attr.mq_msgsize = sizeof(msg_t);
    if ((mqd_listen = mq_open(mq_listen_name, flags, mode, &attr)) == -1)
        exit_err("mq_open: listen");

    // restore umask
    if (umask(omask) == -1)
        exit_err("umask");

    if (is_listen)
        listen_user();
    if (is_invoke)
        invoke_peer();

    return EXIT_SUCCESS;
}

void term_handle(int sig)
{
    pid_t pid;

    // remove message queue if it exist
    if (mq_open(mq_listen_name, O_CREAT | O_EXCL) == -1)
        mq_unlink(mq_listen_name);

    // close peer queue
    close(mqd_peer);

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

int mq_user_name(char *mq_name, char *user)
{
    strcpy(mq_name, MQ_USER_TMP);
    strcat(mq_name, user);

    return 0;
}

void listen_user()
{
    ssize_t rsize;
    msg_t msg;

    // wait for peer send request message with "START"
    // skip for error and continue read
    for (;;) {
        rsize = mq_receive(mqd_listen, (char*)&msg, sizeof(msg), 0);
        if (rsize != sizeof(msg))
            continue;

        if (msg.type != START)
            continue;

        // open peer queue
        strcpy(peer, msg.user);
        mq_user_name(mq_peer_name, peer);
        if ((mqd_peer = mq_open(mq_peer_name, O_WRONLY)) == -1)
            continue;

        // send accept message to peer
        msg.type = ACCEPTED;
        strcpy(msg.user, user);
        if (mq_send(mqd_peer, (char*)&msg, sizeof(msg), 0) == -1) {
            close(mqd_peer);
            continue;
        }

        // success handshake with peer
        break;
    }

    // start message
    start_message();
}

void invoke_peer()
{
    msg_t msg;

    // do not allow self-invoke
    if (strcmp(user, peer) == 0)
        exit_err("Do not allowed self-invoke. User other username");

    // open peer queue
    mq_user_name(mq_peer_name, peer);
    if ((mqd_peer = mq_open(mq_peer_name, O_WRONLY)) == -1)
        exit_err("mq_open");

    // send "START" message
    strcpy(msg.user, user);
    msg.type = START;
    if (mq_send(mqd_peer, (char*)&msg, sizeof(msg), 0) == -1)
       exit_err("mq_send");

    // wait for first "ACCEPTED" message from peer
    printf("invoke %i\n", msg.type);
    if (mq_receive(mqd_listen, (char*)&msg, sizeof(msg), NULL) == -1)
        exit_err("mq_receive");
    if (msg.type != ACCEPTED)
        exit_err("peer denied");

    // start messaging
    start_message();
}

void exit_usage()
{
    printf("Use: <cmd> [-l | -c user]\n");
    exit(EXIT_FAILURE);
}

void start_message()
{
    msg_t msg;
    size_t rsize = sizeof(msg.data);
    char *buf;

    if ((buf = malloc(sizeof(msg.data))) == NULL)
        exit_err("malloc");

    // start new thread to receive message from peer through listen-queue
    // then display to stdout
    if (pthread_create(&receive_thread, NULL, receive_msg_entry, NULL) == -1)
       exit_err("pthread_create");

    // enter infinite loop to read data from command line
    // then send to peer through peer-queue
    for (;;) {
        if (getline(&buf, &rsize, stdin) == -1)
            exit_err("getline");
        strcpy(msg.user, user);
        strcpy(msg.data, buf);
        msg.type = MSG;
        if (mq_send(mqd_peer, (char*)&msg, sizeof(msg), 0) == -1)
            exit_err("mq_send");
    }
}

void *receive_msg_entry(void *argv)
{
    msg_t msg;
    ssize_t rsize;

    for (;;) {
        rsize = mq_receive(mqd_listen, (char*)&msg, sizeof(msg), NULL);
        if (rsize != sizeof(msg))
            warn("mq_receive");

        switch (msg.type) {
            case END:
                printf("peer exit from conversation\n");
                break;
            case ERROR:
                printf("peer is error\n");
                continue;
            case MSG:
                printf("%s: %s\n", msg.user, msg.data);
                break;
            default:
                break;
        }
    }
}

void exit_err(const char *caller)
{
    // terminate thread ?
    pthread_cancel(receive_thread);
    pthread_join(receive_thread, NULL);

    // remove listen-queue if it is exist
    if (mq_open(mq_listen_name, O_CREAT | O_EXCL) == -1)
        mq_unlink(mq_listen_name);

    // close peer-queue
    mq_close(mqd_peer);

    printf("error: %s\n", caller);
    printf("\t%s\n", strerror(errno));
    exit(EXIT_FAILURE);
}
