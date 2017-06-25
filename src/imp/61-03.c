/*
SYNOPSIS

    Solution for exercise 61-03.

REQUIREMENT

    Implement a replacement for sendfile() using read(), write(), and lseek().

SOLUTION

    SENDFILE

        Create new interface of sendfile(), put it to "include/tlpiexer/61-3.h"

            sendfile_x(int out_fd, int in_fd, off_t *offset, size_t count)

        sendfile_x() is implement and put to "src/lib/61-3.c" follow:

            Call fstat() to verify that in_fd refer to regular file.

            If offset is specific, call lseek() to point to required file
            offset.

            Enter loop to read, write data from in_fd to out_fd.

    VERIFY

        Main program creates child process to send file to parent. Parent
        receive it and display to stdout.

        Use internet domain socket library "include/tlpiexer/59-4-isock.h"
        to reduce lines of code.

USAGE

    $ echo "hello there" > tmp/file
    $ ./dest/bin/61-03 tmp/file
    hello there
    child:sendfile_x:12 bytes
    parent:read:12 bytes

AUTHORS

    Kevin Leptons <kevin.leptons@gmail.com>
*/

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <tlpiexer/error.h>
#include <tlpiexer/61-03.h>
#include <tlpiexer/59-04-isock.h>

#define LISTEN_PORT "50001"
#define BACKLOG 8
#define BUF_SIZE 1024

void proc_child(const char *file);
void proc_parent(void);

int main(int argc, char *argv[])
{
    pid_t pid;

    // parse arguments
    if (argc != 2) {
        fprintf(stderr, "Use: %s <file>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // create child to send, parent to receive file
    pid = fork();
    if (pid == FN_ER)
        exit_err("fork");
    else if (pid == 0)
        proc_child(argv[1]);
    else
        proc_parent();
}

void proc_child(const char *file)
{
    int ffd;
    int pfd;
    off_t offset = 0;
    struct stat stat;
    ssize_t tsize;

    // open file
    ffd = open(file, O_RDONLY);
    if (ffd == FN_ER)
        exit_err("child:open");
    if (fstat(ffd, &stat) == FN_ER)
        exit_err("child:stat");

    // connect to receiver
    pfd = isock_connect("localhost", LISTEN_PORT, SOCK_STREAM);
    if (pfd == FN_ER)
        exit_err("child:isock_connect");

    // use sendfile_x() to transfer all of data of file
    tsize = sendfile_x(pfd, ffd, &offset, stat.st_size);
    if (tsize == FN_ER)
        exit_err("child:sendfile_x");

    // success
    close(ffd);
    close(pfd);
    printf("child:sendfile_x:%li bytes\n", tsize);
    exit(EXIT_SUCCESS);
}

void proc_parent(void)
{
    int lfd;
    int pfd;
    char buf[BUF_SIZE];
    ssize_t rsize;
    size_t rsize_total = 0;

    // listen on port
    lfd = isock_listen(LISTEN_PORT, SOCK_STREAM, BACKLOG);
    if (lfd == FN_ER)
        exit_err("parent:isock_listen");

    // accept only peer
    pfd = accept(lfd, NULL, NULL);
    if (pfd == FN_ER)
        exit_err("parent:accept");

    // loop to receive and display data
    for (;;) {
        rsize = read(pfd, buf, sizeof(buf));
        if (rsize == FN_ER)
            exit_err("parent:read");
        if (rsize == 0)
            break;
        write(STDOUT_FILENO, buf, rsize);
        rsize_total += rsize;
    }

    // success
    close(pfd);
    close(lfd);
    printf("parent:read:%lu bytes\n", rsize_total);
}
