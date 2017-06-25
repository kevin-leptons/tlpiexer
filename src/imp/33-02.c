/*
SYNOPSIS

    Solution for exercise 33-02.

REQUIREMENT

    Suppose that a thrad creates a child using fork(). When the child 
    terminates, is it guaranteed that the resulting SIGCHLD signal will be
    delivered to the thread that called fork()? (as opposed to some other
    thread in the process).

SOLUTION

    No guaranteed that SIGCHLD will be delivered to the thread that called
    fork(). Kernel will pick one of threads.

USAGE

    $ ./dest/bin/33-02
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
