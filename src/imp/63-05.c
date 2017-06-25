/*
SYNOPSIS

    Solution for exercise 63-05.

REQUIREMENT

    Modify the program in Listing 63-9 (self_pipe.c) to use poll() instead
    of select().

SOLUTION

    Copy source file from "tlpi/altio/self_pipe.c" to
    "tlpi/altio/self_pipe_63-5.c" and add executable to
    "tlpi/altio/Makefile".

    Modify new source file to use poll() instead of select(). Add few message
    to terminal to inform information. Create loop to monitor in infinite.
    Monitor STDIN_FILENO and SIGUSR1 instead of SIGINT for terminable. 

USAGE

    TERMINAL 1

        # start monitor program in background to see process identity
        $ ./tlpi/altio/self_pipe_63-5 &
        [1] 1254

        # restore program into forground
        fg 1

        # try input to stdin
        hello
        ... => stdin: hello

        # result from terminal 2
        ... => signal: 10 - User defined signal 1

    TERMINAL 2

        # try send signal to monitor program
        # result will appeare in terminal 1
        kill -s SIGUSR1 1254

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
