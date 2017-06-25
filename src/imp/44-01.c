/*
SYNOPSIS

    Solution for exercise 44-01.

REQUIRE

    Write program that uses two pipes to enable bidirectional communication
    between a parent and child process. The prarent process should loop reading
    a block of text from sandard input and use one of the pipes to send the
    text to the child, which converts it to uppercase and sends it back to the
    parent via the other pipe. The parent reads the data coming back from the
    child and echoes it on standard output before continuing around the loop
    once more.

SOLUTION

    - create two pipe by pipe(). pipe-1 use to transfer data from parent 
      process to child process. pipe-2 use to transfer data from child process 
      to parent process
    - create child process by fork()
    - parent process close read end of pipe-1, close write end of pipe-2
    - child process close write end of pipe-1, close read end of pipe-2
    - parent process enter loop to read data from stdin, transfer it to child 
      process though pipe-1. then receive data from child process though
      pipe-2, show it to stdout
    - child process enter loop to receive data from pipe-1, convert string
      to uppercase, then write back to parent process though pipe-2

USAGE

    $ ./dest/bin/44-01
    hello
    parent: HELLO

AUTHORS

    Kevin Leptons <kevin.leptons>
*/

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>

#include <tlpiexer/error.h>

#define BUF_SIZE 4096

// spec     : show help information to stdout
// arg      : none
// ret      : none
void showhelp();

int main(int argc, char *argv[])
{
    // variables
    int pipefd_1[2], pipefd_2[2];
    char buf[BUF_SIZE];
    int rsize;
    int i;

    // show help menu
    if (argc == 2 && strcmp(argv[1], "-h") == 0) {
        showhelp();
        return EXIT_SUCCESS;
    }

    // verify no argumetns has provided
    if (argc > 1) {
        showhelp();
        return EXIT_FAILURE;
    }

    // show infomation
    printf("type string then enter, ctrl+c to exit\n");

    // create pipes
    // pipe-1 for transfer data from parent process to child process
    // pipe-2 for transfer data from child process to parent process
    if (pipe(pipefd_1) == -1)
        exit_err("pipe");
    if (pipe(pipefd_2) == -1)
        exit_err("pipe");

    // create child process to convert string to uppercase
    switch (fork()) {
        case -1:
            // fails
            exit_err("fork");
        case 0:
            // child process continue here
            
            // close unuse file description
            if (close(pipefd_1[1]) == -1)
                exit_err("close");
            if (close(pipefd_2[0]) == -1)
                exit_err("close");
            
            // loop to receive/convert/back string
            for (;;) {
                if ((rsize = read(pipefd_1[0], buf, BUF_SIZE)) == -1)
                    exit_err("read");
                for (i = 0; buf[i] != '\0'; i++)
                   buf[i] = toupper(buf[i]); 
                buf[rsize - 1] = '\0';
                if (write(pipefd_2[1], buf, rsize) == -1)
                    exit_err("write");
            }

            // fall to next statement
            // it will never reach, but this is good design
            break;
        default:
            // parent process continue here
            
            // close unuse file description
            if (close(pipefd_1[0]) == -1)
                exit_err("close");
            if (close(pipefd_2[1]) == -1)
                exit_err("close");

            // loop to get/transfer/get-back/show string
            for (;;) {
                scanf("%s", buf);
                if (write(pipefd_1[1], buf, BUF_SIZE) == -1)
                    exit_err("write");
                if (read(pipefd_2[0], buf, BUF_SIZE) == -1)
                    exit_err("read");
                printf("parent: %s\n", buf);
            }
            
            // fall to next statement
            // it will never reach, but this is good design
            break;
    }

    // success
    return EXIT_SUCCESS;
}

void showhelp()
{
    // variables
    char exename[] = "44-1";

    printf("\nUSAGE:\n\n");
    printf("%s\n", exename);
    printf("%s -h\n\n", exename);
}
