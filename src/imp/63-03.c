/*
SYNOPSIS

    Solution for exercise 63-03.

REQUIREMENT

    Section 63.5 noted that select() can't be used to wait both signal
    and file descriptors, and described a solution using a signal handler
    and pipe. A related problem exists when a program needs to wait for
    input on both file descriptors and System V message queue (since
    System V message queue does not use file descriptors). One solution
    is to fork a separate child process that copies each message from the
    queue to a pipe include among the file descriptors monitored by parent.
    Write a program that uses this scheme with select() to monitor input
    from both the terminal and a message queue.

SOLUTION

    Use threads instead of using child processes.

    Parent process gives child process file descriptor which is bound
    to write-end pipe and message queue name. Child processes monitor
    message queue, read mesage then send via pipe.

    Parent process monitors STDIN_FILENO and read-end pipe by select().
    If data is available on monitored file descriptors, read and display it
    to stdout.

USAGE

    TERMINAL 1

        # create message queues
        $ ./tlpi/pmsg/pmsg_create -c /q1
        $ ./tlpi/pmsg/pmsg_create -c /q2
        $ ./tlpi/pmsg/pmsg_create -c /q3

        # monitor above message queues and stdin of terminal
        $ ./dest/bin/63-03 /q1 /q2 /q3 &> tmp/monitor.log
        New monitored thread: write_fd=4, queue=/q3
        New monitored thread: write_fd=4, queue=/q1
        New monitored thread: write_fd=4, queue=/q2

        # try input to terminal
        hello, I am from terminal
        ... => stdin: hello, I am from terminal

        # output from message queue from terminal 2
        ... => queue: id=6, data=hi, I am from message queue
        ... => queue: id=7, data=hi, other message from queue

    TERMINAL 2

        # try send message to queues
        # output will appears to terminal 1
        $ ./tlpi/pmsg/pmsg_send /q2 "hi, I am from message queue"
        $ ./tlpi/pmsg/pmsg_send /q3 "hi, other message from queue"

AUTHORS

    Kevin Leptons <kevin.leptons@gmail.com>
*/

#include <stdlib.h>
#include <stdio.h>
#include <sys/select.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <stddef.h>

#include <tlpiexer/error.h>

#define MAX_MSG_SIZE 1024

struct thread_entry_arg
{
    int pipe_wfd;
    char queue_name[128];
};

struct queue_msg
{
    int qid;
    ssize_t size;
    char data[MAX_MSG_SIZE];
};

void *monitor_queue_entry(void *arg);
void read_pipe(int fd);
void read_fd(int fd);

int main(int argc, char *argv[])
{
    fd_set read_fdset;
    int max_fd;
    int i;
    int pipe_fd[2];
    struct thread_entry_arg *thread_args;
    pthread_t *threads;
    int result;

    // verify arguments
    if (argc < 2) {
        fprintf(stderr, "Use: %s <msg-queue-name>...\n", argv[0]);
        return EXIT_FAILURE;
    }

    // pipe use for monitor all of message queues
    if (pipe(pipe_fd) != 0)
        exit_err("pipe");

    // start monitor threads
    threads = malloc(sizeof(*threads) * (argc - 1));
    if (threads == NULL)
        exit_err("malloc:threads");
    thread_args = malloc(sizeof(*thread_args) * (argc - 1));
    if (thread_args == NULL)
        exit_err("malloc:thread_args");
    for (i = 0; i < argc - 1; i++) {
        thread_args[i].pipe_wfd = pipe_fd[1];
        strcpy(thread_args[i].queue_name, argv[i + 1]);
        result = pthread_create(threads + i, NULL, monitor_queue_entry,
                                &thread_args[i]);
        if (result != 0)
            exit_err("pthread_create");
    }

    // prepare select data
    max_fd = STDIN_FILENO > pipe_fd[0] ? STDIN_FILENO : pipe_fd[0];
    max_fd += 1;

    for (;;) {
        FD_ZERO(&read_fdset);
        FD_SET(STDIN_FILENO, &read_fdset);
        FD_SET(pipe_fd[0], &read_fdset);

        result = select(max_fd, &read_fdset, NULL, NULL, NULL);
        if (result < 0)
            exit_err("select");
        for (i = 0; i < result; i++) {
            if (FD_ISSET(pipe_fd[0], &read_fdset))
                read_pipe(pipe_fd[0]);
            else if (FD_ISSET(STDIN_FILENO, &read_fdset))
                read_fd(STDIN_FILENO);
        }
    }

    return EXIT_SUCCESS;
}

void *monitor_queue_entry(void *arg)
{
    struct thread_entry_arg entry_arg;
    struct queue_msg qmsg;
    ssize_t wsize;
    struct mq_attr attr;
    ssize_t msg_size;
    char *data;

    entry_arg = *(struct thread_entry_arg*) arg;
    qmsg.qid = mq_open(entry_arg.queue_name, O_RDONLY);
    if (qmsg.qid < 0) {
        msg_err("monitor_queue_entry:mq_open");
        return NULL;
    }

    // allocate data space
    if (mq_getattr(qmsg.qid, &attr) != 0) {
        msg_err("monitor_queue_entry:mq_getattr");
        return NULL;
    }
    data = malloc(attr.mq_msgsize);
    if (data == NULL) {
        msg_err("monitor_queue_entry:malloc");
        return NULL;
    }

    printf("New monitored thread: write_fd=%i, queue=%s\n",
           entry_arg.pipe_wfd , entry_arg.queue_name);

    for (;;) {
        qmsg.size = mq_receive(qmsg.qid, data, attr.mq_msgsize, NULL);
        if (qmsg.size < 0) {
            msg_err("monitor_queue_entry:mq_receive");
            return NULL;
        }
        memcpy(qmsg.data, data, qmsg.size);
        qmsg.data[qmsg.size] = 0;
        msg_size = qmsg.size + offsetof(typeof(qmsg), data);
        wsize = write(entry_arg.pipe_wfd, &qmsg, msg_size);
        if (wsize < 0) {
            msg_err("monitor_queue_entry:write");
            return NULL;
        }
    }
}

void read_pipe(int fd)
{
    ssize_t rsize;
    struct queue_msg qmsg;
    ssize_t header_size;

    header_size = offsetof(typeof(qmsg), data);
    rsize = read(fd, &qmsg, header_size);
    if (rsize != header_size) {
        msg_err("read_pipe:read:header");
        return;
    }

    rsize = read(fd, qmsg.data, qmsg.size);
    if (rsize != qmsg.size) {
        msg_err("read_pipe:read:data");
        return;
    }

    qmsg.data[qmsg.size] = 0;
    printf("... => queue: id=%i, data=%s\n", qmsg.qid, qmsg.data);
}

void read_fd(int fd)
{
    ssize_t rsize;
    char buf[1024];

    rsize = read(fd, buf, sizeof(buf));
    if (rsize < 0) {
        msg_err("read_fd:read");
        return;
    }

    buf[rsize] = 0;
    printf("... => stdin: %s\n", buf);
}
