/*
SYNOPSIS

    Solution for excercise 60-01.

REQUIREMENT

    Add code to the program in Listing 60-4 (is_echo_sv.c) to place a limit
    on the number of simultaneously executing children.

SOLUTION

    Copy source file from "tlpi/sockets/is_echo_sv.c" to
    "tlpi/sockets/is_echo_sv_60-1.c". And add new program to
    "tlpi/sockets/Makefile".

    Use semaphore as synchronize method to limit number of children process

        - Create POSIX semaphore with maximize number of children
        - Parent process must wait semaphore before create child process.
          By this way, value of semaphore reduce to zero and parant process
          can't create more child process.
        - When child process done, increase value of semaphore by 1. So parent
          process can create child process continue.

USAGE

    $ ./dest/bin/60-01
    Goto ... to see solution

AUTHORS

    Kevin Leptons <kevin.leptons@gmail.com>
*/

#include <stdlib.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
    printf("Goto %s to see solution\n", __FILE__);
    return EXIT_SUCCESS;
}
