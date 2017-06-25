/*
SYNOPSIS

    Solution for exercise 09-05.

REQUIREMENT

    Repeat the previous exercise for a process executing a set-user-id-root
    program, which has the following initial set of process credentials.

        real=X effective=0 saved=0

SOLUTION : 

    - reuse program 09-04
    - change owner of program 09-04 to root 
    - enable user-set-id of program 09-04
    - execute that program by non-root user
    - see what is hapend in stdout

USAGE

    # change owner
    $ sudo chown root:root dest/bin/09-04

    # enable user-set-id
    $ chmod u+s,g+s dest/bin/09-04

    # execute with non-root user
    $ ./dest/bin/09-04
    start ids:
    r=1000 e=0 s=0
    suspend: seteuid(r)
    r=1000 e=1000 s=1000
    resume: seteuid(s)
    r=1000 e=0 s=0
    drop permanetly: setresuid(-1, r, r)
    r=1000 e=1000 s=1000
    resume: setresuid(-1, e, s)
    setresuid: Operation not permitted

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
