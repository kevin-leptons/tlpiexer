/*
SYNOPSIS

    Solution for exercise 34-01.

REQUIREMENT

    Suppose a parent process performs the following steps:

        // call fork to create a number of child processes, each of which
        // remains in same process group as the parent

        // some later
        signal(SIGUSR1, SIG_IGN);

        // send signal to children created earlier
        killpg(getprp(), SIGUSR1);

    What problem might be encountered with this application design? (Consider
    shell pipeline). How could this problem can ve avoided?.

SOLUTION

    In shell, kilpg() will send SIGUSR1 to other process in group which
    not creatd by parent if it is exist.

    To avoid this problem, in parent call setpgid() before call fork(). So 
    all of child is created by parent in the same of process group.

USAGE

    $ ./dest/bin/34-01
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
