/*
SYNOPSIS

    Solution for exercise 31-02.

REQUIREMENT

    Use thread-specific data to write thread-safe versions of dirname() and
    basename() (section 18.14).

SOLUTON

    Unneccesary to write dirname(), basename() thread-safe because they are
    not use same static memory.


USAGE

    $ ./dest/bin/32-02

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
