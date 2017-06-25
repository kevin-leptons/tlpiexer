/*
SYNOPSIS

    Solution for exercise 11-01.

REQUIREMENT

    Try running the program in listing 11-1 on other unix implementations
    if you have access to them.

SOLUTION

    - complie the linux programming interface source code distribution in
      tlpi/syslim/t_sysconf.c

    - get other unix implementations and try execute program in 
      tlpi/syslim/t_sysconf

USAGE

    # try to get other UNIX implementations and try run
    # tlpi/syslim/t_sysconf
    $ cd tlpi
    make
    ./syslim/t_sysconf

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
