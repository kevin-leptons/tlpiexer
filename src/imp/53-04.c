/*
SYNOPSIS

    Solution for exercise 53-04.

REQUIREMENT

    In Section 53-5, we noted POSIX semaphores perform much better than
    System V semaphores in the case where the semaphore is uncontented. Write
    two programs (one for each semaphore type) to verify this. Each program
    should simply increment and decrement a semaphore a specified number of
    times. Compare the times required for the two programs.

SOLUTION

    Don't use System V semaphores.

AUTHOR

    Kevin Leptons <kevin.leptons@gmail.com>
*/

#include <stdlib.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
    printf("Goto %s to see solution\n", __FILE__);
    return EXIT_SUCCESS;
}
