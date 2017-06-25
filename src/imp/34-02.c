/*
SYNOPSIS

    Solution for exercise 34-02.

REQUIREMENT

    Write a program to verify that a parent process can change the process 
    group id of one of its children before the child performs an exec(),
    but not afterward.

SOLUTION

    This program will create two child:

        - Child 1 wait for parent change the process group id then exit.

        - Child 2 perform exec(), wait for parent change the process group id 
          then exit. Parent must show error when change that process group id

USAGE

    $ ./dest/bin/34-02
    child_1: start
    parent: change child_1 process group
    child_2: start
    Error: setpgid:Permission denied

AUTHORS

    Kevin Leptons <kevin.leptons@gmail.com>
*/

#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include <tlpiexer/error.h>

// spec     : show help information to stdout
// arg      : none
// ret      : none
void showhelp();

int main(int argc, char **argv)
{
    // variables
    pid_t child_1, child_2;
    int opt;

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

    // create child_1
    // child_1 do not perform exec()
    switch ((child_1 = fork())) {
        case -1:
            exit_err("fork");
        case 0:
            // child_1 process continue here
            printf("child_1: start\n");
            
            // wait for parent perform setpgid()
            sleep(6);
            
            // exit
            _exit(EXIT_SUCCESS);
        default:
            // parent process continue here
            
            // wait for child_1 setup
            sleep(3);
            
            // change process group id of child_1
            if (setpgid(child_1, child_1) == -1)
                exit_err("setpgid");
            printf("parent: change child_1 process group\n");
    }
    
    // create child_2
    // child_2 perform exec()
    switch ((child_2 = fork())) {
        case -1:
            exit_err("fork");
        case 0:
            // child_2 process continue here
            printf("child_2: start\n");
            
            // perform exec()
            // wait for parent change process group
            execlp("sleep", "sleep", "6", (char*)NULL);

            // if process continue here, error has occur
            exit_err("execlp");
        default:
            // parent process continue here
            
            // wait for child_2 perform exec()
            sleep(3);

            // change process group of child_2
            if (setpgid(child_2, child_2) == -1)
                exit_err("setpgid");
            printf("parent: change child_2 process group\n");
    }

    // success
    return EXIT_SUCCESS;
}

void showhelp()
{
    // variables
    char exename[] = "34-2";

    printf("\nUSAGE:\n\n");
    printf("\t%s -h\n\n", exename);
}
