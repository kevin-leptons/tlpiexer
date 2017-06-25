/*
SYNOPSIS

    Solution for exercise 41-01.

REQUIREMENT

    Try compiling a program with and without the -static option, to see the
    differece in size between an executable dynamically linked with the c
    library and one that is linked against the static version of the c library.

SOLUTION

    Compile source code of exercise 40-4 with requirements link to libc in two
    case: use -static option and do not use -static option. performing by
    follow bash command:

        $ gcc -o tmp/40-4.o.dynamic src/40-4.c -lc
        $ gcc -o tmp/40-4.o.static src/40-4.c -lc -static
        $ ls -l tmp | grep 40-4
        -rwxr-xr-x 1 kevin kevin   8528 Aug 22 17:42 40-4.o.dynamic
        -rwxr-xr-x 1 kevin kevin 852064 Aug 22 17:43 40-4.o.static

    We can see that size of 40-4.o.static large than 40-4.o.dynamic because
    static link copy object to output file.

USE

    $ ./dest/bin/41-01
    Goto ... to see solution

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
