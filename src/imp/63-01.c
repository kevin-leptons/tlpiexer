/*
SYNOPSIS

    Solution for exercise 63-01.

REQUIREMENT

    Modify the program in Listing 63-2 (poll_pipes.c) to use select()
    instead of poll().

SOLUTION

    Copy source code from "tlpi/altio/poll_pipes.c" to
    "tlpi/altio/poll_pipes_63-1.c". Add executable file into
    "tlpi/altio/Makefile".
    
    Modify "tpli/altio/poll_pipes_63-1.c" to use select() instead of poll().

USAGE

    $ cd tlpi
    $ make
    $ cd tlpi/altio

    $ ./poll_pipes_63-1 3
    Writing to pipe: 2 (5:6)
    select() returned: 1
    Readable on fd:   5

AUTHORS

    Kevin Leptons <kevin.leptons@gmail.com>
*/

#include <stdlib.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
    printf("Go to %s to see solution\n", __FILE__);
    return EXIT_SUCCESS;
}
