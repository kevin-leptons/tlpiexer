/*
SYNOPSIS

    Solution for excercise 57-03.

REQUIREMENT

    Reimplement the sequence-number server and client of Section 44.8
    using UNIX domain stream sockets.

SOLUTION

    Reimplement server in "src/imp/57-03-server.c".

    Reimplement client in "src/imp/57-03-client.c".

USAGE

    $ ./dest/bin/57-03-server &
    $ ./dest/bin/57-03-client
    0
    $ ./dest/bin/57-03-client
    1
    $ ./dest/bin/57-03-client
    2

AUTHOR

    Kevin Leptons <kevin.leptons@gmail.com>
*/

#include <stdlib.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
    printf("Go to %s to see solution\n", __FILE__);
    return EXIT_SUCCESS;
}
