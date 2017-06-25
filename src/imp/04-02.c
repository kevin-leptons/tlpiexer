/*
SYNOPSIS

    Solution for exercise 04-02

REQUIREMENT

    Write a program like cp that, when used to copy regular file that contains
    holes (sequences of null bytes), also creates corresponding holes in the
    target file

SOLUTION

    Open source file and create new dest file. Copy each block data from 
    source file to dest file until end of source file.

    Note that new file will be create with file mode read-write for owner
    user. Other user can't do nothing with new file. This is limited.
    
USAGE

    # create source file
    $ echo "few of text" > tmp/source.txt

    # copy file
    $ ./dest/bin/04-02 tmp/source.txt tmp/dest.txt

    # retrive new file
    $ cat tmp/dest.txt
    few of text

AUTHORS

    Kevin Leptons <kevin.leptons@gmail.com>
*/

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

#include <tlpiexer/error.h>

#define NEW_FILE_MODE S_IRUSR | S_IWUSR

int main(int argc, char **argv)
{
    char buf[1024];
    ssize_t rsize;
    ssize_t wsize;
    int dest_fd;
    int src_fd;

    // verify arguments
    if (argc != 3) {
        fprintf(stderr, "Use: %s source dest\n", argv[0]);
        return EXIT_FAILURE;
    }

    // source file must be exist
    src_fd = open(argv[1], O_RDONLY);
    if (src_fd < 0)
        exit_err("open:source");

    // dest file must be not exist, it will be create automatically
    dest_fd = open(argv[2], O_WRONLY | O_CREAT | O_EXCL, NEW_FILE_MODE);
    if (dest_fd < 0)
        exit_err("open:dest");

    // copy each file's block from source to dest
    for (;;) {
        rsize = read(src_fd, buf, sizeof(buf));
        if (rsize < 0)
            exit_err("read");
        if (rsize == 0)
            break;

        wsize = write(dest_fd, buf, rsize);
        if (wsize < 0)
            exit_err("write");
    }

    close(src_fd);
    close(dest_fd);
    return EXIT_SUCCESS;
}
