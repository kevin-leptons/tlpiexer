/*
SYNOPSIS

    Solution for exercise 05-05.

REQUIREMENT

    Write a program to verify that duplicated file descriptors share a file
    offset value and open file status flags.

SOLUTION

    - Open an new file to descriptor fdx
    - Use dup() to duplicate file descriptor to fdy
    - Write data to fdx, retrieve file offset and flags.
    - Retrieve fdy's file offset and flags.
    - Write data to fdy, retrieve file offset and flags.

USAGE

    $ ./dest/bin/05-04 tmp/fd.txt
    open: fdx=3
    dup fdx=>fdy: fdy=4
    ... => fdx: size=15, data=data from fdx
    fd=3, offset=15, flags=0
    fd=4, offset=15, flags=0
    ... => fdy: size=15, data=data from fdy
    fd=3, offset=30, flags=0
    fd=4, offset=30, flags=0

AUTHORS

    Kevin Leptons <kevin.leptons@gmail.com>
*/

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>

#include <tlpiexer/error.h>

void fd_info(int fd);

int main(int argc, char **argv)
{
    int fdx;
    int fdy;
    ssize_t wsize;
    const char fdx_data[] = "data from fdx\n";
    const char fdy_data[] = "data from fdy\n";

    if (argc != 2) {
        fprintf(stderr, "Use: %s file\n", argv[0]);
        return EXIT_SUCCESS;
    }

    fdx = open(argv[1], O_CREAT | O_TRUNC | O_WRONLY);
    if (fdx < 0)
        exit_err("open:fdx");
    printf("open: fdx=%i\n", fdx);

    fdy = dup(fdx);
    if (fdy < 0)
        exit_err("dup:fdx=>fdy");
    printf("dup fdx=>fdy: fdy=%i\n", fdy);

    // write data to fdx, retrieve fds info
    wsize = write(fdx, fdx_data, sizeof(fdx_data));
    if (wsize != sizeof(fdx_data))
        exit_err("write:fdx");
    printf("... => fdx: size=%li, data=%s", sizeof(fdx_data), fdx_data);
    fd_info(fdx);
    fd_info(fdy);

    // write data to fdy, retrieve fds info
    wsize = write(fdy, fdy_data, sizeof(fdy_data));
    if (wsize < 0)
        exit_err("write:fdy");
    printf("... => fdy: size=%li, data=%s", sizeof(fdy_data), fdy_data);
    fd_info(fdx);
    fd_info(fdy);

    close(fdx);
    close(fdy);
    return EXIT_SUCCESS;
}

void fd_info(int fd)
{
    int flags;
    off_t offset;

    offset = lseek(fd, 0, SEEK_CUR);
    if (offset < 0)
        exit_err("lseek");

    flags = fcntl(fd, F_GETFD);
    if (flags < 0)
        exit_err("fcntl:F_GETFD");

    printf("fd=%i, offset=%li, flags=%i\n", fd, offset, flags);
}
