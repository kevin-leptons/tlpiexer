/*
SYNOPSIS

    Solution for exercise 04-01

REQUIREMENT

    The tee command reads its standard input until end-of-file, writing a copy
    of the input to standard output and to the file named in its command-line
    argument (We show an example of the use of this command when we discuss 
    FIFOs in section 44.7). Implement tee using I/0 system calls. By default, 
    tee overwrites any existing file with the given name. Implement the -a 
    command-line option (tee -a file), which causes tee to append text to the 
    end of a file if it already exists (Refer to appendix b for a description 
    of the getopt() function, which can be used to parse command-line options).

SOLUTION

    - parse options 
    - verify arguments, options
    - open file to write
    - read from stdin, write to stdout and file
    - close file

USAGE

    # read from standard input then write to stdout and 
    # overwrite to file
    $ ./dest/bin/04-01 tmp/input.txt

    # type few lines to stdin
    hello there
    hello there
    how are you
    how are you

    # press Ctrl + C to terminate process
    # view file result
    $ cat tmp/input.txt
    hello there
    how are you

AUTHORS

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

#define BUF_SIZE 1024

static void showhelp();

int main(int argc, char **argv)
{
    // variables
    char buf[BUF_SIZE];
    ssize_t rsize;
    ssize_t wsize;
    int opt;
    int fd = -1;
    int flags;

    // init
    flags = O_WRONLY | O_TRUNC | O_CREAT;

    // parse options
    while ((opt = getopt(argc, argv, "ah")) != -1) {
        switch (opt) {
            case 'a':
                flags = O_WRONLY | O_APPEND;
                break;
            case 'h':
                showhelp();
                return EXIT_FAILURE;
            default:
                showhelp();
                return EXIT_FAILURE;
        }
    }

    // verify argument as file name
    if (optind >= argc) {
        showhelp();
        return EXIT_FAILURE;
    }

    // open file
    fd = open(argv[optind], flags, S_IRUSR | S_IWUSR);
    if (fd == -1) {
        perror("open");
        return EXIT_FAILURE;
    }

    // read from stdin then write to stdout and file
    for (;;) {
        rsize = read(STDIN_FILENO, buf, BUF_SIZE);
        if (rsize <= 0)
            break; 

        wsize = write(STDOUT_FILENO, buf, rsize);
        if (wsize != rsize)
            exit_err("write:stdout") ;

        wsize = write(fd, buf, rsize);
        if (wsize != rsize)
            exit_err("write:fd");
    }

    // verify read action
    if (rsize == -1)
        exit_err("read");

    // close file
    if (close(fd) == -1)
        exit_err("close");

    // success
    return EXIT_SUCCESS;
}

void showhelp()
{
    // variables
    char exename[] = "4-1";

    printf("\nUSAGE:\n\n");
    printf("\t%s [options] file\n", exename);
    printf("\t%s -h\n\n", exename);

    printf("OPTIONS:\n\n");
    printf("\t-a: append to end of file\n\n"); 
}
