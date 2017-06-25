/*
SYNOPSIS

    Solution for exercise 05-06.

REQUIREMENT

    After each of the calls to write() in the following code, explain what the
    content of the output file would be, and why.

        fd1 = open(file, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
        fd2 = dup(fd1);
        fd3 = open(file, O_RDWR);
        write(fd1, "hello,", 6);
        write(fd2, "world", 6);
        lseek(fd2, 0, SEEK_SET);
        write(fd1, "HELLO", 6);
        write(fd3, "gidday", 6);

SOLUTION

    Write program perform steps as requirement.

    Here are result after each step. Reason is fd1, fd2 refer to one
    open-file-descriptor; fd3 refer to one open-file-descriptor. And they
    aren't share file offset.

        write(fd1, "hello,", 6);        // hello,
        write(fd2, "world", 6);         // hello,world
        lseek(fd2, 0, SEEK_SET);
        write(fd1, "HELLO", 6);         // HELLO,world
        write(fd3, "gidday", 6);        // giddayworld

USAGE

    $ ./dest/bin/05-06 tmp/data.txt
    $ cat tmp/data.txt
    giddayworld

AUTHORS

    Kevin Leptons <kevin.leptons@gmail.com>
*/

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <tlpiexer/error.h>

int main(int argc, char **argv)
{
    int fd1;
    int fd2;
    int fd3;
    ssize_t wsize;

    if (argc != 2) {
        fprintf(stderr, "Use: %s file\n", argv[0]);
        return EXIT_FAILURE;
    }

    fd1 = open(argv[1], O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    if (fd1 < 0)
        exit_err("open:fd1");

    fd2 = dup(fd1);
    if (fd2 < 0)
        exit_err("dup:fd1=>fd2");

    fd3 = open(argv[1], O_RDWR);
    if (fd3 < 0)
        exit_err("open:fd3");

    wsize = write(fd1, "hello,", 6);
    if (wsize != 6)
        exit_err("write:fd1");

    wsize = write(fd2, "world", 6);
    if (wsize != 6)
        exit_err("write:fd2");

    if (lseek(fd2, 0, SEEK_SET) < 0)
        exit_err("lseek:fd2");

    wsize = write(fd1, "HELLO", 6);
    if (wsize != 6)
        exit_err("write:fd1");

    wsize = write(fd3, "gidday", 6);
    if (wsize != 6)
        exit_err("write:fd3");

    // success
    return EXIT_SUCCESS;
}
