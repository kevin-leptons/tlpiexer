/*
SYNOPSIS

    Solution for excercise 59-03.

REQUIREMENT

    Write UNIX domain sockets library with an API similar to the Internet
    domain sockets library shown in Section 59.12. Rewrite the programs
    in Listing 57-3 (us_xfr_sv.c, on page 1168) and Listing 57-4
    (us_xfr_cl.c, on page 1169) to use this library.

SOLUTION

    Re-design APIs and put it into "include/tlpiexer/59-03.h".

    Implement APIs in "src/lib/59-03.c".

    Rewrite us_xfr_sv in "src/imp/59-03-server.c" and us_xfr_cl
    in "src/imp/59-03-client.c".

USAGE

    $ ls > tmp/dump.txt
    $ ./dest/bin/59-03-server > tmp/dump-copy.txt &
    $ ./dest/bin/59-03-client < tmp/dump.txt
    $ kill %1
    $ diff tmp/dump.txt tmp/dump-copy.txt

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
