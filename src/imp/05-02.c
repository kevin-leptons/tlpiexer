/*
SYNOPSIS

    Solution for exercise 05-02.

REQUIREMENT

    Write a program that opens an existing file for writing with the O_APPEND
    flag, and then seeks to the beginning of the file before writing some data.
    Where does the data appear in the file? why?

SOLUTION

    - open file specify from command line
    - seek to end of file
    - write sample data to file
    - seek to begin of file
    - close file

USAGE

    # create data file
    $ echo "few of words" > tmp/data.txt

    # write, seek with file same as requirements
    $ ./dest/bin/05-02 tmp/data.txt

    # retrieve data of file
    $ cat tmp/data.txt
    few of words
    some-data-here

AUTHORS

    Kevin Leptons <kevin.leptons@gmail.com>
*/

#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <getopt.h>
#include <errno.h>

#include <tlpiexer/error.h>

#define buf_size 1024

void showhelp();

int main(int argc, char **argv)
{
    // variables
    int fd;
    char fdata[] = "some-data-here\n";

    // verify argument
    if (argc != 2 || strcmp(argv[1], "-h") == 0) {
        showhelp();
        return EXIT_FAILURE;
    }

    // open file
    fd = open(argv[1], O_WRONLY | O_APPEND, S_IRUSR, S_IWUSR);
    if (fd == -1)
        exit_err("open");

    // write sample data to file
    if (write(fd, fdata, sizeof(fdata)) == -1)
        exit_err("write");

    // seek to begin of file
    if (lseek(fd, 0, SEEK_SET) == -1)
        exit_err("lseek");

    // close file
    if (close(fd) == -1)
        exit_err("close");

    return EXIT_SUCCESS;
}

void showhelp()
{
    // variables
    char exename[] = "5-2";

    printf("\nUSAGE:\n\n");
    printf("\t%s file\n", exename);
    printf("\t%s -h\n\n", exename);
}
