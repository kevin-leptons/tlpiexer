/*
SYNOPSIS

    Solution for excercise 59-02.

REQUIREMENT

    Modify the programs in Listing 59-6 (is_seqnum_sv.c) and Listing 59-7
    (is_seqnum_cl.c) to use the inetListen() and inetConnect() functions
    provided Listing 59-9 (inet_sockets.c).

SOLUTION

    Copy source files

        "tlpi/sockets/is_seqnum_sv.c" => "tlpi/sockets/is_seqnum_sv_59-02.c"
        "tlpi/sockets/is_seqnum_cl.c" => "tlpi/sockets/is_seqnum_cl_59-02.c"

    Add new program into "tlpi/sockets/Makefile".

    Use inetListen() and inetConnect() to reduce lines of code with these
    two new program.

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
