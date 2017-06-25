/*
SYNOPSIS

    Solution for exercise 11-02.

REQUIREMENT

    Try running the program in listing 11-2 on other file systems.

SOLUTION : 

    - complie the linux programming interface source code distribution in
      tlpi/

    - get other file systems and try execute program in
      tlpi/syslim/t_fpathconf

USAGE     :

    # try to get other file systems and run program
    $ cd tlpi
    $ make
    $ ./syslim/t_fpathconf

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
