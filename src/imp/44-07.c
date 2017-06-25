/*
SYNOPSIS

    Solution for exercise 44-07.

REQUIREMENT

    Write programs to verify operations of nonblocking opens and nonblocking
    I/O on FIFOs (see Section 44.9).

SOLUTION

    Exercise 44-6 have been proof O_NONBLOCK flag. At server, it open client
    FIFO with O_NONBLOCK flag to avoid block if client have not create FIFO

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
