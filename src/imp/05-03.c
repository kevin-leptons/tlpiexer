/*
SYNOPSIS

    Solution for exercise 05-03.

REQUIREMENT

    This exercise is designed to demontrate why the atomicity guaranteed by
    opening a file with the O_APPEND flag is necessary. Write a program that 
    takes up to three command-line arguments.

        $ atomic_append filename num-byte [x]

    This file should open the specified filename (creating it if neccessary) 
    and num-bytes to the file by using write() to write a byte at a time. 
    By default, the program should open the file with the O_APPEND flag, 
    but if a third command-line argument (x) is supplied, then the O_APPEND 
    flag should be omitted, and instead the program should perform an 
    lseek(fd, 0 SEEK_END) call before each write(). Run two instances of this 
    program at the same time without the x argument to write 1 million bytes 
    to the same file:

        $ atomic_append f1 1000000 & atomic_append f1 1000000

    Repeat the same steps, writing to a different file, but this time 
    specifying the x argument.

        $ atomic_append f2 1000000 x & atomic_append f2 1000000 x

    List the sizes of the file f1 and f2 using ls -l and explain the 
    difference.

SOLUTION

    - verify argument
    - open file with mode
    - loop to write sample data to file
    - close file

    As below usage, size of f2.txt smaller 2,000,000 bytes because each
    process open same file independently, it create two open-file-descriptors
    and not shared same file offset. Then two process write to same file
    without any synchronization, data from this process can overwrite other
    process. That is reason why after done, size of f2.txt smaller 2,000,000.

USAGE

    # open file with O_APPEND flag and write data
    $ ./dest/bin/05-03 tmp/f1.txt 1000000 & \
    ./dest/bin/05-03 tmp/f1.txt 1000000

    # open file without O_APPEND, seek to end of file end write data
    $ ./dest/bin/05-03 tmp/f2.txt 1000000 x & \
    ./dest/bin/05-03 tmp/f2.txt 1000000 x

    # see difference
    $ ls -l tmp/f1.txt tmp/f2.txt
    -rw------- 1 kevin kevin 2000000 Jun  9 23:08 tmp/f1.txt
    -rw------- 1 kevin kevin 1015456 Jun  9 23:08 tmp/f2.txt

AUTHORS

    Kevin Leptons <kevin.leptons@gmail.com>
*/

#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
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
    char fdata[] = "some-data-here\n";
    int fmode;
    bool usex = false;
    ssize_t total_wsize;
    ssize_t to_wsize;
    ssize_t wsize;

    // verify argument
    if (argc < 3 || strcmp(argv[1], "-h") == 0) {
        showhelp();
        return EXIT_FAILURE;
    }

    // chose open file mode
    if (argc == 4 && strcmp(argv[3], "x") == 0) {
        fmode =  O_CREAT | O_WRONLY;
        usex = true;
    } else {
        fmode = O_CREAT | O_WRONLY | O_APPEND;
    }

    // open file
    fd = open(argv[1], fmode, S_IRUSR | S_IWUSR);
    if (fd == -1)
        exit_err("open");

    // seek
    if (usex) {
        if (lseek(fd, 0, SEEK_END) == -1)
            exit_err("lseek");
    }

    // write to file
    total_wsize = atoll(argv[2]);
    for (; total_wsize > 0;) {
        to_wsize = total_wsize > sizeof(fdata) ? sizeof(fdata) : total_wsize;
        wsize = write(fd, fdata, to_wsize);
        if (wsize != to_wsize)
            exit_err("write");
        total_wsize -= wsize;
    }

    // close file
    if (close(fd) == -1)
        exit_err("close");

    // success
    return EXIT_SUCCESS;
}

void showhelp()
{
    // variables
    char exename[] = "5-3";

    printf("\nUSAGE:\n\n");
    printf("%s file nbyte [x]\n", exename);
    printf("%s -h\n\n", exename);

    printf("ARGUMENTS:\n\n");
    printf("x: open file without O_APPEND flag\n\n"); 
}
