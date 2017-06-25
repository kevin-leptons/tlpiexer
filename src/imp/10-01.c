/*
SYNOPSIS

    Solution for exercise 10-01.

REQUIREMENT

    Assume a system where the value returned by the call sysconf(_SC_CLK_TCK)
    is 100. Assuming that the clock_t value returned by times() is an unsigned
    32-bit integer, how long will it take before this value cycles so that it 
    restart at 0? perform the same calculation for the CLOCKS_PER_SEC value
    returned by clock().

SOLUTION

    Unimplemented.

USAGE

AUTHORS

    Kevin Leptons <kevin.leptons@gmail.com>
*/

#include <stdlib.h>
#include <stdio.h>

int main(int argc, char **argv)
{
    printf("Goto %s to see solution\n", __FILE__);
    return EXIT_SUCCESS;
}
