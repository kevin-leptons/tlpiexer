/*
SYNOPSIS

    Solution for excercise 57-01.

REQUIREMENT

    In Section 57.3, we noted that UNIX domain datagram sockets are reliable.
    Write programs to show that if a sender transmits datagrams to a UNIX
    domain datagram socket faster than the receiver reads them, then the
    sender is eventually blocked, and remains blocked util the receiver reads
    some of the pending datagrams.

SOLUTION

    Use parent process as writer, child process as reader.

    In writer:

        Create lsock_fd.

        Limit socket buffer size to SOCK_BUF_SIZE.

        Bind lsock_fd to WRITER_SOCK_PATH.

        Wait for ready message from reader.

        Enter loop to write MSG_NUM messages with size
        BUF_SIZE = 2 * SOCK_BUF_SIZE to reader. In second writting, it will
        be block because reader pause reading for READER_PAUSE_SECS seconds.

    In reader:

        Create lsock_fd.

        Bind lsock_fd to READER_SOCK_PATH

        Wait for READER_WAIT_SECS seconds for writer's constructing.

        Send ready message to writer.

        Enter loop to read MSG_NUM messages from writer.
        In first message, pause reading for READER_PAUSE_SECS so writting
        will be block. After pausing, messages is send and write as normal.

USAGE

    # start program
    # when reader pause, writer is block in first time
    # and read continue

    $ ./dest/bin/57-01
    constructing: SOCK_BUF_SIZE=2048; BUF_SIZE=4096; PAUSE_SECS=8
    reader:pause reading
    writer:write 0:4096 bytes
    reader:resume reading

    reader:read 0:4096 bytes
    writer:write 1:4096 bytes
    reader:read 1:4096 bytes
    writer:write 2:4096 bytes
    reader:read 2:4096 bytes
    writer:write 3:4096 bytes
    reader:read 3:4096 bytes

AUTHORS

    Kevin Leptons <kevin.leptons@gmail.com>
*/

#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <string.h>

#include <tlpiexer/error.h>

#define SOCK_BUF_SIZE 2048
#define BUF_SIZE SOCK_BUF_SIZE * 2
#define MSG_NUM 4
#define READER_WAIT_SECS 1
#define READER_PAUSE_SECS 4
#define WRITER_SOCK_PATH "/tmp/tlpiexer-57-01-writer"
#define READER_SOCK_PATH "/tmp/tlpiexer-57-01-reader"

void read_proc();
void write_proc();

int main(int argc, char *argv[])
{
    pid_t pid;

    // disable stdout buffer to ensure correct output
    setbuf(stdout, NULL);
    setbuf(stderr, NULL);

    printf("constructing: SOCK_BUF_SIZE=%u; BUF_SIZE=%u; PAUSE_SECS=%u\n",
            SOCK_BUF_SIZE, BUF_SIZE, READER_PAUSE_SECS);
    pid = fork();
    if (pid == -1)
        exit_err("fork");
    else if (pid == 0) {
        read_proc(); // child process as reader
    }
    else {
        write_proc(); // parent process as writer
    }
}

void read_proc()
{
    int lsock_fd;
    int res;
    int i;
    struct sockaddr_un lsock_addr;
    struct sockaddr_un psock_addr;
    ssize_t tsize;
    char buf[BUF_SIZE];

    memset(buf, 0, sizeof(buf));

    // construct addresses
    memset(&lsock_addr, 0, sizeof(lsock_addr));
    lsock_addr.sun_family = AF_UNIX;
    strcpy(lsock_addr.sun_path, READER_SOCK_PATH);

    memset(&psock_addr, 0, sizeof(psock_addr));
    psock_addr.sun_family = AF_UNIX;
    strcpy(psock_addr.sun_path, WRITER_SOCK_PATH);

    // create local socket
    lsock_fd = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (lsock_fd == FN_ER)
        exit_err("reader:socket");

    // remove path same as socket path
    res = remove(READER_SOCK_PATH);
    if (res != FN_OK && errno != ENOENT)
        exit_err("reader:remove");

    // bind local socket to address
    res = bind(lsock_fd, (struct sockaddr*) &lsock_addr, sizeof(lsock_addr));
    if (res != FN_OK)
        exit_err("reader:bind");

    // wait for writer's constructing
    sleep(READER_WAIT_SECS);

    // send message to writer to start writing
    strcpy(buf, "hello there");
    tsize = sendto(lsock_fd, buf, sizeof(buf), 0,
                   (struct sockaddr*) &psock_addr, sizeof(psock_addr));
    if (tsize != sizeof(buf))
        exit_err("reader:sendto");

    // start read message from writer until its close socket
    for (i = 0; i < MSG_NUM; i++) {
        // delay reading to see that writer is block when buffer is full
        if (i == 0) {
            printf("reader:pause reading\n");
            sleep(READER_PAUSE_SECS);
            printf("reader:resume reading\n\n");
        }
        tsize = recvfrom(lsock_fd, buf, sizeof(buf), 0, NULL, NULL);
        if (tsize == -1)
            exit_err("reader:recvfrom");
        printf("reader:read %i:%lu bytes\n", i, tsize);
    }

    // notify writer that reading was done
    sendto(lsock_fd, buf, sizeof(buf), 0,
           (struct sockaddr*) &psock_addr, sizeof(psock_addr));

    close(lsock_fd);
    exit(EXIT_SUCCESS);
}

void write_proc()
{
    int lsock_fd;
    int res;
    int i;
    char buf[BUF_SIZE];
    struct sockaddr_un lsock_addr;
    struct sockaddr_un psock_addr;
    socklen_t psock_len;
    ssize_t tsize;
    int opt[1] = { SOCK_BUF_SIZE };

    memset(buf, 0, sizeof(buf));

    // create socket
    lsock_fd = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (lsock_fd == -1)
        exit_err("writer:socket");

    // limit size of buffer of socket
    res = setsockopt(lsock_fd, SOL_SOCKET, SO_SNDBUF, opt, sizeof(opt));
    if (res != FN_OK)
       exit_err("writer:setsocktopt");

    // remove with the same name with socket
    res = remove(WRITER_SOCK_PATH);
    if (res != FN_OK && errno != ENOENT)
        exit_err("writer:remove");

    // setup address
    memset(&lsock_addr, 0, sizeof(lsock_addr));
    lsock_addr.sun_family = AF_UNIX;
    strcpy(lsock_addr.sun_path, WRITER_SOCK_PATH);

    // bind socket to address
    res = bind(lsock_fd, (struct sockaddr*)&lsock_addr, sizeof(lsock_addr));
    if (res != FN_OK)
        exit_err("writer:bind");

    // wait for a message from reader to start writing
    psock_len = sizeof(psock_addr);
    tsize = recvfrom(lsock_fd, buf, sizeof(buf), 0,
                     (struct sockaddr*) &psock_addr, &psock_len);
    if (tsize == -1)
        exit_err("writer:recvfrom");

    // enter loop to write data to reader
    strcpy(buf, "hello you!");
    for (i = 0; i < MSG_NUM; i++) {
        tsize = sendto(lsock_fd, buf, sizeof(buf), 0,
                      (struct sockaddr*) &psock_addr, psock_len);
        if (tsize != sizeof(buf))
            exit_err("writer:sendto");
        printf("writer:write %i:%lu bytes\n", i, tsize);
    }

    // wait for a message from reader to free socket
    recvfrom(lsock_fd,  buf, sizeof(buf), 0,
                     (struct sockaddr*) &psock_addr, &psock_len);

    close(lsock_fd);
    exit(EXIT_SUCCESS);
}
