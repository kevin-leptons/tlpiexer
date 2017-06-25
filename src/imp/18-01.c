/*
SYNOPSIS

    Solution for exercise 18-01.

REQUIREMENT

    In section 4.3.2, we noted that it is not possible to open a file for
    writing if it is currently being executed (open() returns -1, with errno
    set to ETXTBSY). Nevertheless, it is possible to do following from the
    shell.

        $ cc -o longrunner longrunner.c
        $ # leave running in background
        $ ./longrunner &                    
        $ # make some changes to the source code
        $ vi longrunner.c
        $ cc -o longrunner longrunner.c

    The last command overwrites the existing executable of the same name.
    How is this possible? (for a clue, use ls -li to look the i-node number
    of the executable file after each compliation).

SOLUTION

    Unimplemented

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
