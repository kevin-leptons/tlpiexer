/*
SYNOPSIS

    Solution for exercise 39-01.

REQUIREMENT

    Modify the program in example 35-2 (sched_set.c, on page 743) to use 
    file capabilities, so that it can be used by an unprivilieged user.

SOLUTION

    Do not modify source to use file capabilities. It only must change file
    capability. Instructions below.

        $ # use sample program 
        $ cp tlpi/procpri/sched_set tmp/
        $ cp tlpi/procpri/sched_view tmp/

        $ # change file capability to allow it use sched_setscheduler()
        $ sudo setcap "cap_sys_nice=pe" tmp/sched_set

        $ # try use it without privilege
        $ sleep 128 &
        [1] 10920
        $ # set SCHED_FIFO, priority = 25 for process id 10920
        $ ./tmp/sched_set f 25 10920

        $ # review
        $ ./tmp/sched_view 10920
        10920: FIFO 25

USAGE

    $ ./dest/bin/39-01
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
