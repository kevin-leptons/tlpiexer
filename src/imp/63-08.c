/*
SYNOPSIS

    Solution for exercise 63-08.

REQUIREMENT

    Modify the program in Listing 63-3 (demo_sigio.c) to use a realtime
    signal instead of SIGIO. Modify the signal handler to accept a siginfo_t
    argument and display the values of the si_fd and si_code fields of this
    structure.

SOLUTION

    Copy source file from "tlpi/altio/demo_sigio.c" to
    "tlpi/altio/demo_sigio_63-8.c" and add excutable to
    "tlpi/altio/Makefile".

    Modify program such as requirement's hints.

USAGE

    # start program
    $ ./tlpi/altio/demo_sigio_63-8

    # press 'x' three times then press '#' to terminate process
    cnt=1; read x
    ... => signal: signo=37: si_fd=0, si_code=1
    cnt=10; read x
    ... => signal: signo=37: si_fd=0, si_code=1
    cnt=14; read x
    ... => signal: signo=37: si_fd=0, si_code=1
    cnt=23; read #
    ... => signal: signo=37: si_fd=0, si_code=1

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
