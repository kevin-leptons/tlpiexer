/*
SYNOPSIS

    Solution for exercise 05-01.

REQUIREMENT

    Modify the program in listing 5-3 to use standard file I/O system calls
    (open() and lseek()) and the off_t data type. compile program with the
    _FILE_OFFSET_BITS macro set to 64, and test it to show that a large file
    can be successfuly created.

SOLUTION

    - open file specify from command line
    - seek to position specify from command line
    - write sample data
    - close

USAGE

    # create sample file
    $ echo "few of text" > tmp/data.txt

    # write sample data to file at offset
    $ ./dest/bin/05-01 tmp/data.txt 7

    # retrieve data of file above
    $ cat tmp/data.txt
    few of some data here

AUTHORS

    Michael Kerrisk
    Kevin Leptons <kevin.leptons@gmail.com>
*/

#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <getopt.h>
#include <sys/stat.h>

#include <tlpiexer/error.h>

void showhelp();

int main(int argc, char **argv)
{
    // variables
    int fd;
    off_t off;
    const char fdata[] = "some-data-here";

    // verify argument
    if (argc != 3 || strcmp(argv[1], "-h") == 0) {
        showhelp();
        return EXIT_FAILURE;
    }

    // open file
    fd = open(argv[1], O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
    if (fd == -1)
        exit_err("open");

    // seek to specific
    off = atoll(argv[2]);
    if (lseek(fd, off, SEEK_SET) == -1)
        exit_err("lseek");

    // write sample data
    if (write(fd, fdata, sizeof(fdata)) == -1)
        exit_err("write");

    // close file
    if (close(fd) == -1)
        exit_err("close");

    // success
    return EXIT_SUCCESS;
}

void showhelp()
{
    // variables
    const char exename[] = "5-1";

    printf("\nUSAGE:\n\n"); 
    printf("\t%s file offset\n\n", exename);
}
