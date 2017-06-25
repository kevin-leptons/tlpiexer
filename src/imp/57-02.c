/*
SYNOPSIS

    Solution for excercise 57-02.

REQUIREMENT

    Rewrite the programs in Listing 57-3 (us_xfr_sv.c) and Listing 57-4 
    (us_xfr_cl.c) to use Linux-specific abstract socket namespace
    Section 57.6).

SOLUTION

    Copy source code from "tlpi/sockets/us_xfr_sv.c" to
    "tlpi/sockets/us_xfr_sv_57-2.c" and add program to 
    "tlpi/sockets/Makefile"

    Copy source code from "tlpi/sockets/us_xfr_cl.c" to
    "tlpi/sockets/us_xfr_cl_57-2.c" and add program to 
    "tlpi/sockets/Makefile"

    Set sun_path[0] to NULL, follow by path terminate by NULL. 
    It's look like below.

        -----------------------------------
        | 0 | / | p | a | t | h | 0 | ... |
        -----------------------------------

USAGE 

    $ cd tlpi/sockets
    $ make
    $ ls > ../../tmp/dump.txt
    $ ./us_xfr_sv_57-2 > ../../tmp/dump-copy.txt &
    $ ./us_xfr_cl_57-2 < ../../tmp/dump.txt
    $ kill %1
    $ diff ../../tmp/dump.txt ../../tmp/dump-copy.txt
    
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
