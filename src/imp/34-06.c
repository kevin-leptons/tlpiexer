/*
SYNOPSIS

    Solution for exercise 34-06.

REQUIREMENT

    Write a program to verify that when a process in an orphaned process group
    attempts to read() from the controlling terminal, the read() fails with
    the error EIO.

SOLUTION

    - Create process in orphaned process group. That can be done by create
      child process, set it to new process group. Then create grandchild
      process, and child process exit imediately. Grandchild process will
      become process in orphaned process group
    - Call read(). It must fails with the error EIO

USAGE

    $ ./dest/bin/34-06
    wait a moment, error will occur
    $ grandhilcd: pid=15434
    grandhicld: pgid=15433
    Error: read:Input/output error

AUTHORS

    Kevin Leptons <kevin.leptons@gmail.com>
*/

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

#include <tlpiexer/error.h>

#define BUF_SIZE 1024

// spec     : show help information to stdout
// arg      : none
// ret      : none
void showhelp();

int main(int argc, char *argv[])
{
    // variables
    int opt;
    int rsize;
    char buf[BUF_SIZE];

    // parse options
    while ((opt = getopt(argc, argv, "h")) != -1) {
        switch (opt) {
            case 'h':
                showhelp();
                return EXIT_SUCCESS;
            default:
                showhelp();
                return EXIT_FAILURE;
        }
    }

    // verify no arguments has provided
    if (optind > argc) {
        showhelp();
        return EXIT_FAILURE;
    }

    // do not use buffer for stdout
    setbuf(stdout, NULL);

    // show information
    printf("wait a moment, error will occur\n");

    // create child process
    // and set it to new process group
    switch (fork()) {
        case -1:
            // fails
            exit_err("fork");
        case 0:
            // child process continue here
            
            // set itself to new process group
            if (setpgid(0, 0) == -1)
                exit_err("setpgid");
            
            // create grandchild process
            switch (fork()) {
                case -1:
                    // fails
                case 0:
                    // grandchild process continue here
                    
                    // wait for child process exit
                    // grandchild become orphaned process
                    sleep(3);

                    // show information
                    printf("grandhilcd: pid=%ld\n", (long)getpid());
                    printf("grandhicld: pgid=%ld\n", (long)getpgrp());

                    // perform read()
                    // it must fails with error EIO
                    if ((rsize = read(STDIN_FILENO, buf, sizeof(buf)) == -1))
                        exit_err("read");

                    // read() success
                    // it must never reach
                    printf("some thing went wrong, read %i bytes\n", rsize);

                    // exit
                    _exit(EXIT_SUCCESS);
                default:
                    // child process continue here
                    
                    // exit to make grandchild process become orphaned process
                    // group
                    _exit(EXIT_SUCCESS);
            }
        default:
            // parent process continue here
            
            // do nothing, fall to next statement 
            break; 
    }

    // wait for all of child process exited
    wait(NULL);

    // success
    return EXIT_SUCCESS;
}

void showhelp()
{
    // variables
    char exename[] = "34-6";

    printf("\nUSAGE:\n\n");
    printf("%s\n", exename);
    printf("%s -h\n\n", exename);
}
