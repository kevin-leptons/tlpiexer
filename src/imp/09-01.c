/*
SYNOPSIS

    Solution for exercise 09-01.

REQUIREMENT

    Assume in each of the following cases that the initial set of process user
    ids is real=1000 effective=0 saved=0 file-system=0. What would be the state
    of the user ids after following calls?.

        a) setuid(2000);
        b) setreuid(-1, 2000);
        c) seteuid(2000);
        d) setfsuid(2000);
        e) setresuid(-1, 2000, 3000);

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
