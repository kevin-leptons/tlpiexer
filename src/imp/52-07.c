/*
SYNOPSIS

    Solution for exercise 52-07.

REQUIREMENT

    In Listing 52-7, could buffer be mad a global variable and its meomory
    allocated just once (in the main program) ?. Explain your answear.

SOLUTION

    Yes, "buffer" could be located in main() and its memory allocated once
    time.  Then pass "buffer" to notifySetup(). In notifySetup(), put
    "buffer", "mqdp" into new data structure called thread_argv_t and pass its
    pointer to sev.sigev_value.sival_prt. Then in threadFunc(), thread_argv_t
    can be received and use.

USAGE

    $ ./dest/bin/52-07
    Goto ... to see solution

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
