/*
SYNOPSIS

    Solution for exercise 13-01.

REQUIREMENT

    Using the time built-in command of the shell, try timing the operation of
    the program in listing 4-1 (copy.c) on your system:

     a) experiment with different file and buffer sizes. you can set the buffer
        size using the -DBUF_SIZE=nbytes option when compiling the program

     b) modify the open() system call to include the O_SYNC flag. how much
        difference does this make to the speed for various buffer sizes?

     c) try performing these timing tests on a range of file systems 
        (e.g., ext3, XFS, Btrfs and JFS). are the result similar? are the
        trends the same when going from small to large buffer sizes?

SOLUTION

    Unimplement.

USAGE

    $ ./dest/bin/13-01
    Goto ... to see solution

AUTHORS

    Kevin Leptons <kevin.leptons@gmail.com>
*/

#include <stdlib.h>
#include <stdio.h>

int main(int argc, char **argv)
{
    printf("Goto %s to see solution\b", argv[0]);
    return EXIT_SUCCESS;
}
