/*
SYNOPSIS

    Solution for exercise 63-04.

REQUIREMENT

    The last step of the description of the self-pipe  technique in
    Section 63.5.2 started that the program should first drain the pipe,
    and then perform any actions that should be taken in response to the
    signal. What might happen if these substeps were reversed?

SOLUTION

    It isn't different because signal can arrive at anytime during drain
    pipe and perform action. That mean all that's signal can be drain.
    This is a mistake.

USAGE

    $ ./dest/bin/63-04
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
